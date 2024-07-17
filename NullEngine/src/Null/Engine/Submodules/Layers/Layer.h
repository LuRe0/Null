#pragma once

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// File Name:	Layer.h
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
	class NLE_API ILayer
	{
	public:
		ILayer() = default;

		virtual ~ILayer() {};

		virtual void OnAttach() {};

		virtual void OnUpdate(float dt) {};

		virtual void OnRender(float dt) {};

		virtual void OnDetach() {};


		virtual void OnEvent(const Event& event) {};

	private:
		ILayer(ILayer const&) = delete;
		ILayer& operator=(ILayer const&) = delete;
	};

	template<typename T>
	class NLE_API Layer : public ILayer
	{
	public:
		static std::string TypeName() {
			std::string fullName = typeid(T).name();
			// Manual parsing: adjust based on compiler output
			auto pos = fullName.find_last_of(':');
			return fullName.substr(pos + 1);
		}
	private:


	};


	//class LayerStack
	//{
	//public:
	//	LayerStack()
	//	{}
	//	~LayerStack()
	//	{}

	//	void AddLayer(std::unique_ptr<ILayer>&& layer);

	//	void PopLayer();

	//private:
	//	std::vector<std::unique_ptr<ILayer>> m_LayerList;

	//	LayerStack(LayerStack const&);
	//	LayerStack& operator=(LayerStack const&);
	//};
}