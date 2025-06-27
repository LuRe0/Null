//Particle flags need to match c++ ones

const uint PARTICLE_FLAG_NONE                  = 0u;

const uint PARTICLE_FLAG_ROTATION_OVER_LIFETIME = 1u << 0u;
const uint PARTICLE_FLAG_SCALE_OVER_LIFETIME    = 1u << 1u;
const uint PARTICLE_FLAG_COLOR_OVER_LIFETIME    = 1u << 2u;
const uint PARTICLE_FLAG_VELOCITY_OVER_LIFETIME = 1u << 3u;
const uint PARTICLE_FLAG_RANDOMIZE_LIFETIME      = 1u << 4u;

const uint PARTICLE_FLAG_FORCES                 = 1u << 5u;

const uint PARTICLE_FLAG_CUSTOM_LOGIC           = 1u << 6u;
const uint PARTICLE_FLAG_COLLISION_ENABLED      = 1u << 7u;
const uint PARTICLE_FLAG_ALPHA_OVER_LIFETIME    = 1u << 8u;


uniform uint u_EmitterFlags;