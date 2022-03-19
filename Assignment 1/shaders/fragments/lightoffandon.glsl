#pragma once
#include "frame_uniforms.glsl"


vec3 lightonoff(vec3 Light) {
    //Enable Light when Flag is Enabled
    if (IsFlagSet(FLAG_ENABLE_DISABLE_LIGHTS))
    {
       return Light;
    }
    else 
    {
        return vec3(0,0,0);
    }

    //Turn off if it is Disabled
}
