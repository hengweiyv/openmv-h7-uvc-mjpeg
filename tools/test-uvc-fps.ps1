param(
    [string] $FfmpegPath = 'ffmpeg',
    [string] $CameraName = 'OpenMV UVC in FS Mode',
    [int] $RequestedFps = 30,
    [string[]] $Sizes = @('320x240', '352x288', '400x300', '480x320'),
    [int] $Frames = 600,
    [int] $TimeoutSeconds = 40
)

if ($RequestedFps -le 0 -or $Frames -le 0 -or $TimeoutSeconds -le 0) {
    throw 'RequestedFps、Frames 和 TimeoutSeconds 必须为正整数。'
}

$ffmpeg = (Get-Command -Name $FfmpegPath -ErrorAction Stop).Source

foreach ($size in $Sizes) {
    if ($size -notmatch '^\d+x\d+$') {
        throw "无效尺寸：$size"
    }

    $startInfo = [System.Diagnostics.ProcessStartInfo]::new($ffmpeg)
    $startInfo.UseShellExecute = $false
    $startInfo.CreateNoWindow = $true
    $startInfo.RedirectStandardOutput = $true
    $startInfo.RedirectStandardError = $true

    $arguments = @(
        '-nostdin', '-hide_banner', '-loglevel', 'error',
        '-f', 'dshow', '-vcodec', 'mjpeg',
        '-video_size', $size, '-framerate', [string] $RequestedFps,
        '-i', "video=$CameraName",
        '-frames:v', [string] $Frames,
        '-progress', 'pipe:1', '-nostats',
        '-f', 'null', 'NUL'
    )
    foreach ($argument in $arguments) {
        [void] $startInfo.ArgumentList.Add($argument)
    }

    $timer = [System.Diagnostics.Stopwatch]::StartNew()
    $process = [System.Diagnostics.Process]::Start($startInfo)
    # 同时排空两个管道，避免 FFmpeg 进度输出阻塞造成假性掉帧。
    $stdoutTask = $process.StandardOutput.ReadToEndAsync()
    $stderrTask = $process.StandardError.ReadToEndAsync()
    $completed = $process.WaitForExit($TimeoutSeconds * 1000)
    if (-not $completed) {
        $process.Kill($true)
        $process.WaitForExit()
    }
    $timer.Stop()

    $stdout = $stdoutTask.GetAwaiter().GetResult()
    $stderr = $stderrTask.GetAwaiter().GetResult()
    $frameMatches = [regex]::Matches($stdout, '(?m)^frame=(\d+)\s*$')
    $observedFrames = if ($frameMatches.Count) {
        [int] $frameMatches[$frameMatches.Count - 1].Groups[1].Value
    } else {
        0
    }
    $errorSummary = ($stderr -split "`r?`n" | Where-Object { $_ } | Select-Object -First 2) -join ' | '

    [pscustomobject]@{
        requested_fps = $RequestedFps
        pixel_format = 'mjpeg'
        size = $size
        frames = $observedFrames
        elapsed_s = [math]::Round($timer.Elapsed.TotalSeconds, 3)
        measured_fps = [math]::Round($observedFrames / [math]::Max($timer.Elapsed.TotalSeconds, 0.001), 2)
        complete = $completed -and $process.ExitCode -eq 0 -and $observedFrames -eq $Frames
        error = $errorSummary
    } | ConvertTo-Json -Compress
}
