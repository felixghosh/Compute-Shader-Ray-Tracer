# Benchmarks
## r7 7700x + gtx 1080
800 * 800 pixels, depth of 3, no subsampling:
CPU: 260 ms
OPEN_GL: 53 ms (avg. fps 19) 

Different color size the image applies gamma correction, which the computer shader one doesn't.
It's worth mentioning that the framerate differns depnding on where in the scene we are, 29 fps when looking at diffuse objects only (minium amout of rays), and 9 when filing the screen with transparent sphere which has both a reflection and refraction component. The performance scales almost linearly to the amout of rays, 3/8 ≃ 9/29

## i7 8550u + intel UHD Graphics 620
800 * 800 pixels, depth of 3, no subsampling:
CPU: 550 ms
OPEN_GL: 125 ms (avg. fps 8)

Interseting how the the little performance scaling compared to thier fp perforance.
