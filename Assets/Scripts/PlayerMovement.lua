
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local PlayerMovement = {
    data = 
    {
        Speed = { value = 250, serialize = true },
        Accel = { value = vec2(0, 0), serialize = false },
        Jerk =  { value = vec2(0, 0), serialize = false  },
        startPos =  { value = -475, serialize = false },
    }
}

setmetatable(PlayerMovement, {
    __index = function(t, k)
        if PlayerMovement.data[k] then
            return PlayerMovement.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if PlayerMovement.data[k] then
            PlayerMovement.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function PlayerMovement:Start()
    -- print("PlayerMovement started")
    -- Initialization code here
end

function PlayerMovement:Update(dt)
    -- print("PlayerMovement started")
    local rb2d = pEntity:get_component(Rigidbody2D)
    local transform = pEntity:get_component(Transform)
    local translation = transform.translation

    transform:set_translation(vec3(translation.x, PlayerMovement.startPos, translation.z))
    -- Trace.debug("Here") 
    MoveBox()

    PlayerMovement.Accel = PlayerMovement.Accel * 1.0 / (1.0 + dt * rb2d.linear_damping)

    PlayerMovement.Accel = PlayerMovement.Accel + (PlayerMovement.Jerk*dt)
    local velocity = rb2d.linear_velocity + (PlayerMovement.Accel*dt)

    rb2d:set_linear_velocity(velocity)
end

function PlayerMovement:Exit()
    -- print("PlayerMovement exited")
    -- Cleanup code here
end


function MoveBox()

    if(Input.KeyDown(KEY_D)) then
        PlayerMovement.Jerk.x = PlayerMovement.Speed
    elseif Input.KeyDown(KEY_A) then
        PlayerMovement.Jerk.x = -(PlayerMovement.Speed)
    else
        PlayerMovement.Jerk.x = 0
    end

    if PlayerMovement.Jerk.x ~= 0 or PlayerMovement.Jerk.y ~= 0 then
        PlayerMovement.Jerk = normalize(PlayerMovement.Jerk)
    end

    
    PlayerMovement.Jerk = PlayerMovement.Jerk * PlayerMovement.Speed
    
    
end



function PlayerMovement:OnCollisionEnter(otherEntity)
    -- Check if otherEntity is valid

    if otherEntity then

        if(otherEntity:has_component(Transform)) then

            local sprite = pEntity:get_component(Sprite)

            if sprite then
                local r = Random.range_float(0.1,1.0)
                local g = Random.range_float(r,1.0)
                local b = Random.range_float(g,1.0)
                local newtint = vec4(r, g, b, 1)
                sprite:set_tint(newtint)
            end
    
        end
    end


    -- Trace.debug("Type of otherEntity: {0}", type(otherEntity)) 
end

return PlayerMovement
