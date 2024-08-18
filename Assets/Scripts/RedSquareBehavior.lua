
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local RedSquareBehavior = {
    data = 
    {
        Speed = { value = 250, serialize = true },
        Accel = { value = vec2(0, 0), serialize = true },
        Jerk =  { value = vec2(0, 0), serialize = true }
    }
}

setmetatable(RedSquareBehavior, {
    __index = function(t, k)
        if RedSquareBehavior.data[k] then
            return RedSquareBehavior.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if RedSquareBehavior.data[k] then
            RedSquareBehavior.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function RedSquareBehavior:Start()
    -- print("RedSquareBehavior started")
    -- Initialization code here
end

function RedSquareBehavior:Update(dt)
    -- Update code here
    local rb2d = pEntity:get_component(Rigidbody2D)
    MoveBox()
    -- Trace.debug("Here")

    RedSquareBehavior.Accel = RedSquareBehavior.Accel * 1.0 / (1.0 + dt * rb2d.linear_damping)

    RedSquareBehavior.Accel = RedSquareBehavior.Accel + (RedSquareBehavior.Jerk*dt)
    local velocity = rb2d.linear_velocity + (RedSquareBehavior.Accel*dt)

    rb2d:set_linear_velocity(velocity)

    -- RedSquareBehavior.Accel = vec2(0,0);
end

function RedSquareBehavior:Exit()
    -- print("RedSquareBehavior exited")
    -- Cleanup code here
end

function MoveBox()
    if(Input.KeyDown(KEY_W)) then
        -- Trace.debug("here")
        RedSquareBehavior.Jerk.y = RedSquareBehavior.Speed
    elseif Input.KeyDown(KEY_S) then
        RedSquareBehavior.Jerk.y = -(RedSquareBehavior.Speed)
    else
        RedSquareBehavior.Jerk.y = 0
    end


    if(Input.KeyDown(KEY_D)) then
        RedSquareBehavior.Jerk.x = RedSquareBehavior.Speed
    elseif Input.KeyDown(KEY_A) then
        RedSquareBehavior.Jerk.x = -(RedSquareBehavior.Speed)
    else
        RedSquareBehavior.Jerk.x = 0
    end

    if RedSquareBehavior.Jerk.x ~= 0 or RedSquareBehavior.Jerk.y ~= 0 then
        RedSquareBehavior.Jerk = normalize(RedSquareBehavior.Jerk)
    end

    
    RedSquareBehavior.Jerk = RedSquareBehavior.Jerk * RedSquareBehavior.Speed
    
    
end

return RedSquareBehavior
