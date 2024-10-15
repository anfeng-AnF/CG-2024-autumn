#include "AWangshuInn.h"
#include "UStaticMeshComponent.h"

AWangshuInn::AWangshuInn(Graphics& gfx)
{
	std::string filePath = "Models\\WangShukz\\Hall.pmx";

	auto StaticMeshConponent = std::make_shared<UStaticMeshComponent>();
	auto staticMesh =std::make_shared<StaticMesh>(gfx, filePath);
	staticMesh->SetShader<Bind::PixelShader>("Phong2DPS.cso", "");
	staticMesh->SetShader<Bind::VertexShader>("Phong2DVS.cso", "");

	StaticMeshConponent->SetStaticMesh(staticMesh);
	this->AddComponent(RootComponentName, StaticMeshConponent, UStaticMeshComponent::name);

	this->Transform = FTransform();
	Transform.scale = XMVectorSet(1,1,1,0);
}

void AWangshuInn::Render(Graphics& Gfx)
{
	Super::Render(Gfx);
}
