## TODO
- [ ] How to handle outter and inner ita at surface?
- [x] obj opengl viewer
- [ ] ObjHandler still has problem when file contains texure/normal indiceis.
- [ ] use both translate/rotate or position/lookat/up to set camera frame
- [ ] some homogeneous coord are not normalized, may cause bugs! Homogeneous coord w may not be exactly 1 when multiplied with projection matrcies, so we need to normalize the point back to w=1. (Q: does the normalization changes the position in coord sys, if true, the point is not the same point?!)
- [ ] row/column major matrix mechanism seems to be used unapproapiately in the system, need to check!
- [ ] check if primitives from .obj files are located in their object spaces(at origin in world space) or already in world space. This is important since we have ["transform"] attribute in scene files.
- [ ] Json parse exception handler
- [ ] camera rays still have problem!!!! When to change camera ray directions? when camera.z < 0, error!(x,y become inf). is this related with screen_window? when camera.z > 0, camera ray direction opposite!!
Also, do we flip (pos->lookat) vector, so that camera space forward vector do not account for camera ray direction (instead do flip in generateRay() function)?
- [x] BSDF
- [ ] Exception Class
- [x] screen window aspect ratio? (ortho window size control?)
- [ ] FrConductor complex number calculation?
- [x] specular conductor & dielectric, ior spectrum or float?

# Overall Design
Rendering jobs are handled by `Renderer` instances. A `Renderer` object holds the pointer of the integrator and the pointer of the scene to be rendered.

Buffers are managed by `Sensor` class objects. When write buffers to disks, `Renderer` object fetches buffers from `Sensors` objects, and call corresponding IO handler to write.
Rendering settings are stored in `RenderingSettings` class objects.

Use **Row-Major** operations!

# Class References
## Coordinate
Use right-hand coordinate.
x-axis ascends from left to right;
y-axis ascends from bottom to up;
z-axis ascends from in to out;

- [ ] It seems that coordinate systems built from normals, are different from the ones build from location vectors, given the `z` direction.
> In conclusion, it is tradition to use up vector as z-axis in shading related coordinates, while others use forward vector as z-axis.

## Transform
Always use `Vec3f` * `Mat4f` to apply transformations.
**Tranform matrices are identical for points and vectors. The difference lies in the homogeneous coord of the points and the vectors themselves.
Thus we do not need much attention when computing matrices.**
- [ ] Should I distinguish points and vectors as it relates to different homogeneous coordinate?

## Integrator
`Integrator` objects hold pointer to rendered scene.
For current compliance with pbrt-v3, put all intersection related infor, e.g. BSDF, surface info, in `isect`.
- [ ] Is `Visibility Tester` really necessary?

### Whitted Integrator
- [ ] If whitted doesn't account indirect lighting, then why bounce?

## Camera
-[ ] moving camera
When creating `Camera` objects, default values for `_pos`, `_lookat`, `_up` are (0,0,0), (0,1,0), (0,0,1).

Camera is oriented along **negative z-axis** by default, thus z-coordinates need to be divided by **-z** when doing perspective projection.
- [ ] (ortho projection?)

### Camera related spaces
- world space(3d)
- camera space(3d): where camera is facing **negative** z-axis. All visible points' coordinates have negative z coord.
- screen space(2d): image plane space, it is still confusing to me *whether this space is a 3d space or 2d*. All the coord lies in screen window, i.e. [-1,1]. Note that z coords are flipped and thus positive!
> ok. screen space is a 3d space, the z coordinates seems to be used for depth-of-field simulation. They are not considered when camera generats rays, we only considers x and y coordinates when genereating rays.
- NDC space(2d): used to normalize coordinates in screen space to [0,1].
- raster space(2d): unnormalize coordinates in NDC space to fit film spatial ration.

`orthographic` or `perspective` projection matrix projects objects in camera space to the **near plane** in camera space, not screen space. screen space is defined on the film plane(where raster space is).

### Camera Default Parameters
- distance between `image plane` and `camera(eye)` is **1.0**.
- screen window is set to [-1,1] at default.

## Primitive
`Primitive` class holds geometric primitive implementations. `Homura` does not distinguish between *Shapes* and *Primitives* like pbrt-v3 does, instead it place BSDF properties in `Primitive` objects.

`Primitive` objects always use **world coordinate**!

### TriangleMesh
Use CCW to represent vertices.
#### Triangle Intersection
1. directly solve the coordinate transformation from linear equation.
- [ ]translate-permute-shear
- [ ]what is error bound used for?

## Rays
### Ray Differentials
Ray differentials are just extension of `Ray` class to contain more information of original ray, including two differential rays.
Only the routines that need to account for **antialiasing** and **texturing** require RayDifferential parameters.

## BxDF & BSDF
So it seems the bsdf coordinates are stored in `BSDF` instances, instead of `IntersectInfo`. So the `_shading` member of `IntersectInfo` stores world space vectors.

- [ ] check if wo is properly handled! `pointing out` & `coord transform`.

### BxDF
Every vector manipulated by `BxDF` class instances, are in **reflection coordinate**!

Note `_R` is computed by `Texture` instances.

- [ ] Which classes have `Sample_f()`?
- [ ] Why ray from whether **light source** or **camera** should be distinguished for `SpecularTransmission`?

### BSDF
- holds all BxDF instances
- manage coordinate transforms

Since every `BSDF` is linked with a specific intersection event, `Material` does not own a BSDF.

As for the `MatchFlags()` function, it requires **all** attributes in queried BSDF can be found in provided flags.
- [ ] How to decompose BSDFs? Why when part of attributes in BSDF belong to provided flags, the BSDF cannot be considered a composing BxDF?

- [ ] What if I do not use `dpdu` as tangent vector? Is it ok to use any vector in the triangle plane?
> Reﬂection computations in pbrt are evaluated in a reﬂection coordinate system where the two tangent vectors and the normal vector at the point being shaded are aligned with the x, y, and z axes, respectively. (p509, pbrt)

So what the normal is not aligned with z-axis? How to transform?
If frame basis are all exactly aligned with [x,y,z]-axis, how can z-axis represents normal? Or "aligned" just mean they are (1,0,0), (0,1,0) and (0,0,1) in the shading coord?

## Material
Represents surface shaders.

So `Material` instances manage `BSDF` and `Texture` to give final shading effects.

# Emitters
Luminous efficacy

## IO
Note `.obj` file indicies starts from **1**!

**NOTE: `Box` class is not a `Primitive`!**

## Questions
- [ ] matrix multiplication order?
- [ ] signed edge function, relationship with barycentric coord?
- [ ] how does the order of ei in signed edge function matter?
- [ ] inverse matrix Gaussian-Jordan
- [ ] error log out
- [ ] buffer vs film?
- [ ] smart pointer
- [ ] google glog library
- [ ] ray differentials
- [ ] C++ hierarcical classes constructor behavior
- [ ] What is screen_window used for?