## Reflection

diffuse, glossy specular, perfect specular, retro-reflective.

### Reflection Coordinate System
This coordinate system is aligned with world coordinate system.

### Shading coordinate system
Defined as spherical system!
> Why shading coordinate should be aligned with world coordinates systems's x,y,z-axis? If a point on a sphere surface is to be shaded, should the shading coordinate's x-y plane be rotated to align with the tangent plane at that point?

Surface normals will always facing outward the surface.

### Hemispherical Reflection Function
What are hemispherical reflection functions used for?

### Compute reflection direction & amount

Fresnel Reflectance
> How does it relate to polarization?

## Materials
### Dielectric
Transmit & Reflect incident illumination.

`ior` is guaranteed to be real-valued!

### Conductor
It can actually transmit an extremely small portion of energy but absorbed rapidly. Only thin enough conductors can show such property. Thus transmision is ignored.

Also note that the `ior` is complex value!???

> How does complex/real-valued `ior` influence appearance?