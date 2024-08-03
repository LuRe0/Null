player_behavior = 
{
    moveSpeed = 5.0,
    health = 100,
    name = "Player1",
    isAlive = true
}

function player_behavior:Start()
    -- print("Player behavior started")
    -- Initialization code here
end

function player_behavior:Update(dt)
    local transform = ParentEntity:GetTransform()
    if transform then
        print(transform.Translation.x, transform.Translation.y, transform.Translation.z)
    else
        print("TransformComponent not found!")
    end
end

function player_behavior:Exit()
    -- print("Player behavior exited")
    -- Cleanup code here
end
 
return player_behavior
