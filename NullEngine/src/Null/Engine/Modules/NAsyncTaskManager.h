#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	NAsyncTaskManager.h
// Author(s):	name
// 
//------------------------------------------------------------------------------

//******************************************************************************//
// Includes																        //
//******************************************************************************//
#include "Null/Core.h"
#include "Null/Engine/Modules/Base/IModule.h"


//******************************************************************************//
// Definitions  														        //
//******************************************************************************//



//******************************************************************************//
// Private constants														    //
//******************************************************************************//

//******************************************************************************//
// Private structures													        //
//******************************************************************************//


namespace NULLENGINE
{

	struct IAsyncTask
	{
		virtual ~IAsyncTask() = default;
		virtual void Process() = 0;
		virtual bool IsComplete() const = 0;
	};


	class NLE_API NAsyncTaskManager : public ModuleBase<NAsyncTaskManager>
	{
	public:

		void Load() override;
		void Init() override;
		void Update(float dt) override;
		void RenderLoadScreen();
		void Unload() override;
		void Shutdown() override;

		void EnqueueTask(std::unique_ptr<IAsyncTask> task);
		bool HasPendingTasks() const;

		bool HasRenderImGui() const override { return true; }


		void RenderImGui() override;

	private:
		std::queue<std::unique_ptr<IAsyncTask>> m_TaskQueue;

		 int m_TasksToProcess = 2;

		 int m_TotalTasks = 0;
		 int m_TasksProcessed = 0;
	};

}