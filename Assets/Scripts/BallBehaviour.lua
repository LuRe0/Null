
-- THIS IS THE EQUIVALENT TO THE CLASS DEFINITION. SET SERIALIZED FIELDS TO TRUE TO SHOW IN EDITOR 
local BallBehaviour = {
    data = 
    {
        speed = { value = 750, serialize = true },
        boost = { value = 10, serialize = true },
        boostDuration = { value = 2, serialize = true },

        tempSpeed = { value = 0, serialize = false },
        elapsedTime = { value = 0, serialize = false }
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

    BallBehaviour.elapsedTime = BallBehaviour.elapsedTime + dt

    
    local t = BallBehaviour.elapsedTime / BallBehaviour.boostDuration



    if t > 1.0 then
        t = 1.0
    end

    if vel.x ~= 0 or vel.y ~= 0 then
        vel = normalize(vel)
        vel = vel * BallBehaviour.speed
    else
        vel = b2Vec2(0, 0)  
    end

    vel = vel * (BallBehaviour.speed + BallBehaviour.tempSpeed)
    -- Trace.debug("{0}, {1}", vel.x, vel.y)
    rb2d:set_linear_velocity(vel)

    BallBehaviour.tempSpeed = lerp(BallBehaviour.tempSpeed, 0, t)
end

function BallBehaviour:Exit()
    -- print("BallBehaviour exited")
    -- Cleanup code here
end


function BallBehaviour:OnCollisionEnter(otherEntity)
    -- Check if otherEntity is valid

    if otherEntity then
        if(otherEntity:has_component(Rigidbody2D)) then
            if(otherEntity.name == "Player") then
                local otherRb2d = otherEntity:get_component(Rigidbody2D)
                local rb2d = pEntity:get_component(Rigidbody2D)
                local vel = rb2d.linear_velocity
                local otherVel = otherRb2d.linear_velocity;
                local newVel = vel + otherVel;

                if newVel.x ~= 0 or newVel.y ~= 0 then
                    newVel = normalize(newVel)
                    newVel = newVel * (BallBehaviour.speed +BallBehaviour.tempSpeed)
                else
                    newVel = b2Vec2(0, 0)  -- Or set to an appropriate default direction
                end

                newVel = newVel * (BallBehaviour.speed + BallBehaviour.tempSpeed) * 1.5

                -- Trace.debug("here")

                rb2d:set_linear_velocity(newVel)
       
            
                BallBehaviour.tempSpeed = BallBehaviour.boost;
                BallBehaviour.elapsedTime = 0;
            else
                     
                BallBehaviour.tempSpeed = BallBehaviour.boost/2;
                BallBehaviour.elapsedTime = 0;
            end
        end
    end
end


return BallBehaviour
