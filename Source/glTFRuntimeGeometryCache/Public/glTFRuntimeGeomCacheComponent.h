// Copyright 2025, Roberto De Ioris.

#pragma once

#include "CoreMinimal.h"
#include "GeometryCacheComponent.h"
#include "glTFRuntimeGeomCacheComponent.generated.h"

/**
 *
 */
UCLASS(ClassGroup = (Rendering), meta = (BlueprintSpawnableComponent, DisplayName = "glTFRuntime Geometry Cache Component"))
class GLTFRUNTIMEGEOMETRYCACHE_API UglTFRuntimeGeomCacheComponent : public UGeometryCacheComponent
{
	GENERATED_BODY()
public:

	//~ Begin UPrimitiveComponent Interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	//~ End UPrimitiveComponent Interface.
};
