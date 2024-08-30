
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local BallBehaviour = {
    data = 
    {
        speed = { value = 500, serialize = true }
    }
}

setmetatable(BallBehaviour, {
    __index = function(t, k)
        if BallBehaviour.data[k] then
            return BallBehaviour.data[k].value
        else
            return nil
        end
    end,
    __newindex = function(t, k, v)
        if BallBehaviour.data[k] then
            BallBehaviour.data[k].value = v
        else
            rawset(t, k, v)
        end
    end
})

function BallBehaviour:Start()
    -- print("BallBehaviour started")
    -- Initialization code here
end

function BallBehaviour:Update(dt)
    local rb2d = pEntity:get_component(Rigidbody2D)
    local vel = rb2d.linear_velocity

    if vel.x ~= 0 or vel.y ~= 0 then
        vel = normalize(vel)
        vel = vel * BallBehaviour.speed
    else
        vel = b2Vec2(0, 0)  -- Or set to an appropriate default direction
    end

    vel = vel * BallBehaviour.speed
    -- Trace.debug("{0}, {1}", vel.x, vel.y)
    rb2d:set_linear_velocity(vel)

end

function BallBehaviour:Exit()
    -- print("BallBehaviour exited")
    -- Cleanup code here
end


function BallBehaviour:OnCollisionEnter(otherEntity)
    -- Check if otherEntity is valid

    if otherEntity then
        if(otherEntity:has_component(Rigidbody2D)) then
            local otherRb2d = otherEntity:get_component(Rigidbody2D)
            local rb2d = pEntity:get_component(Rigidbody2D)
            local vel = rb2d.linear_velocity
            local otherVel = otherRb2d.linear_velocity;
            local newVel = vel + otherVel;

            if newVel.x ~= 0 or newVel.y ~= 0 then
                newVel = normalize(newVel)
                newVel = newVel * BallBehaviour.speed
            else
                newVel = b2Vec2(0, 0)  -- Or set to an appropriate default direction
            end

            newVel = newVel * BallBehaviour.speed * 1.5

            -- Trace.debug("here")

            rb2d:set_linear_velocity(newVel)
        end
    end
end


return BallBehaviour
