
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local RedSquareBehavior = {
    data = 
    {
        Speed = { value = 10, serialize = true },
        Accel = { value = 50, serialize = true },
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
    local velocity = vec2(0)
    local rb2d = pEntity:get_component(Rigidbody2D)

    local linearVel = rb2d.linear_velocity
    
    if(Input.KeyDown(KEY_UP)) then
        -- Trace.debug("here")
        velocity.y = RedSquareBehavior.Speed
    elseif Input.KeyDown(KEY_DOWN) then
        velocity.y = -(RedSquareBehavior.Speed)
    else
        velocity.y = 0
    end


    if(Input.KeyDown(KEY_RIGHT)) then
        velocity.x = RedSquareBehavior.Speed
    elseif Input.KeyDown(KEY_LEFT) then
        velocity.x = -(RedSquareBehavior.Speed)
    else
        velocity.x = 0
    end
    
    -- Trace.debug("v_x: {0}, v_y: {1}", velocityX, velocityY)

    rb2d:set_linear_velocity(velocity)


end

function RedSquareBehavior:Exit()
    -- print("RedSquareBehavior exited")
    -- Cleanup code here
end

function easeInOutQuint(x)
    if x < 0.5 then
        return 16 * x * x * x * x * x
    else
        return 1 - math.pow(-2 * x + 2, 5) / 2
    end
end

return RedSquareBehavior
