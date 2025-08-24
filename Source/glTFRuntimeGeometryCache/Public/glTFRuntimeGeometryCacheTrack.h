// Copyright 2025, Roberto De Ioris.

#pragma once

#include "CoreMinimal.h"
#include "GeometryCacheTrack.h"
#include "GeometryCacheMeshData.h"
#include "glTFRuntimeGeometryCacheTrack.generated.h"

/**
 *
 */
UCLASS()
class GLTFRUNTIMEGEOMETRYCACHE_API UglTFRuntimeGeometryCacheTrack : public UGeometryCacheTrack
{
	GENERATED_BODY()

public:
	virtual const bool UpdateBoundsData(const float Time, const bool bLooping, const bool bIsPlayingBackward, int32& InOutBoundsSampleIndex, FBox& OutBounds) override;
	virtual const FGeometryCacheTrackSampleInfo& GetSampleInfo(float Time, const bool bLooping) override;
	virtual const bool UpdateMeshData(const float Time, const bool bLooping, int32& InOutMeshSampleIndex, FGeometryCacheMeshData*& OutMeshData) override;
	virtual bool GetMeshDataAtSampleIndex(int32 SampleIndex, FGeometryCacheMeshData& OutMeshData) override;
	virtual bool GetMeshDataAtTime(float Time, FGeometryCacheMeshData& OutMeshData) override;

	bool IsTopologyCompatible(const int32 SampleIndexA, const int32 SampleIndexB) const;
	int32 GetSampleInfoIndex(const float Time, const bool bLooping);

	void AddMeshSample(const FGeometryCacheMeshData& InMeshData, const TArray<UMaterialInterface*>& InMeshMaterials, const float SampleTime);

	TSet<UMaterialInterface*> GetUniqueMeshMaterials() const;

	void UpdateMeshMaterialsIndices(const TArray<UMaterialInterface*>& InGlobalMaterials);

	int32 GetNumSampleInfos() const;

	float GetMeshSampleTime(const int32 SampleIndex) const;

protected:
	TArray<FGeometryCacheTrackSampleInfo> SampleInfos;

	TArray<FGeometryCacheMeshData> MeshSamples;
	TArray<float> MeshSampleTimes;

	TArray<TArray<UMaterialInterface*>> MeshMaterialsSamples;

	FGeometryCacheMeshData MeshData;
};
