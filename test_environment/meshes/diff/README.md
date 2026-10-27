# Diff robot meshes

The GLB files are the source assets. Gazebo Classic 11 does not load GLB, so
`body.dae`, `wheel_l.dae`, and `wheel_r.dae` are used by the robot description.
The conversion preserves scene transforms and material base colors. The body
uses millimetres (URDF scale 0.001); the wheels use metres (URDF scale 1).

When regenerating COLLADA, give every source/array/vertices element a unique ID
across the entire document and update its references. Duplicate IDs produced
by multi-mesh exporters cause Gazebo to read the wrong vertex arrays and can
crash its Ogre renderer. Preserve the source GLB scene transforms when exporting.
