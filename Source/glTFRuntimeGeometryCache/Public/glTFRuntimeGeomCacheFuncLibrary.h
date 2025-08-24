// Copyright 2025, Roberto De Ioris.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "glTFRuntimeGeometryCacheTrack.h"
#include "glTFRuntimeAsset.h"
#include "GeometryCache.h"
#include "glTFRuntimeGeomCacheFuncLibrary.generated.h"

USTRUCT(BlueprintType)
struct FglTFRuntimeGeometryCacheFrame
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "glTFRuntime|GeometryCache")
	float Time;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "glTFRuntime|GeometryCache")
	FTransform Transform;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "glTFRuntime|GeometryCache")
	FglTFRuntimeMeshLOD Mesh;
};

/**
 *
 */
UCLASS()
class GLTFRUNTIMEGEOMETRYCACHE_API UglTFRuntimeGeomCacheFuncLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "glTFRuntime|GeometryCache")
	static UGeometryCache* LoadGeometryCacheFromRuntimeTracks(const TArray<UglTFRuntimeGeometryCacheTrack*>& Tracks, const int32 FPS = 24);

	UFUNCTION(BlueprintCallable, Category = "glTFRuntime|GeometryCache")
	static UglTFRuntimeGeometryCacheTrack* LoadRuntimeTrackFromGeometryCacheFrames(const TArray<FglTFRuntimeGeometryCacheFrame>& Frames);

};
