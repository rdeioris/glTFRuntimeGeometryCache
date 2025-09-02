// Copyright 2025, Roberto De Ioris.


#include "glTFRuntimeGeomCacheComponent.h"
#include "GeometryCacheSceneProxy.h"
#include "GeometryCacheHelpers.h"
#include "glTFRuntimeGeometryCacheTrack.h"

class FglTFRuntimeGeomCacheTrackProxy : public FGeomCacheTrackProxy
{
public:
	FglTFRuntimeGeomCacheTrackProxy(ERHIFeatureLevel::Type InFeatureLevel)
		: FGeomCacheTrackProxy(InFeatureLevel)
	{
	}

	//~ Begin FGeomCacheTrackProxy Interface
	virtual bool UpdateMeshData(float Time, bool bLooping, int32& InOutMeshSampleIndex, FGeometryCacheMeshData& OutMeshData) override
	{
		if (UglTFRuntimeGeometryCacheTrack* RuntimeTrack = Cast<UglTFRuntimeGeometryCacheTrack>(Track))
		{
			FGeometryCacheMeshData* PtrMeshData = nullptr;
			const bool bResult = RuntimeTrack->UpdateMeshData(Time, bLooping, InOutMeshSampleIndex, PtrMeshData);
			if (bResult)
			{
				OutMeshData = *PtrMeshData;
			}
			return bResult;
		}
		return false;
	}

	virtual bool GetMeshData(int32 SampleIndex, FGeometryCacheMeshData& OutMeshData) override
	{
		if (UglTFRuntimeGeometryCacheTrack* RuntimeTrack = Cast<UglTFRuntimeGeometryCacheTrack>(Track))
		{
			return RuntimeTrack->GetMeshDataAtSampleIndex(SampleIndex, OutMeshData);
		}
		return false;
	}

	virtual bool IsTopologyCompatible(int32 SampleIndexA, int32 SampleIndexB) override
	{
		if (UglTFRuntimeGeometryCacheTrack* RuntimeTrack = Cast<UglTFRuntimeGeometryCacheTrack>(Track))
		{
			return RuntimeTrack->IsTopologyCompatible(SampleIndexA, SampleIndexB);
		}
		return false;
	}

	virtual const FVisibilitySample& GetVisibilitySample(float Time, const bool bLooping) const override
	{
		return FVisibilitySample::VisibleSample;
	}

	virtual void FindSampleIndexesFromTime(float Time, bool bLooping, bool bIsPlayingBackwards, int32& OutFrameIndex, int32& OutNextFrameIndex, float& InInterpolationFactor) override
	{
		if (UglTFRuntimeGeometryCacheTrack* RuntimeTrack = Cast<UglTFRuntimeGeometryCacheTrack>(Track))
		{
			const int32 ThisSampleIndex = RuntimeTrack->GetSampleInfoIndex(Time, bLooping);

			OutFrameIndex = ThisSampleIndex;
			OutNextFrameIndex = OutFrameIndex + 1;

			float AdjustedTime = Time;
			if (bLooping)
			{
				AdjustedTime = GeometyCacheHelpers::WrapAnimationTime(Time, RuntimeTrack->GetDuration());
			}

			const float FrameIndexTime0 = RuntimeTrack->GetMeshSampleTime(OutFrameIndex);
			const float FrameIndexTime1 = RuntimeTrack->GetMeshSampleTime(OutNextFrameIndex);
			InInterpolationFactor = (AdjustedTime - FrameIndexTime0) / (FrameIndexTime1 - FrameIndexTime0);
			if (bIsPlayingBackwards)
			{
				Swap(OutFrameIndex, OutNextFrameIndex);
				InInterpolationFactor = 1.0f - InInterpolationFactor;
			}
		}
	}
	//~ End FGeomCacheTrackProxy Interface
};

class FglTFRuntimeGeometryCacheSceneProxy final : public FGeometryCacheSceneProxy
{
public:
	FglTFRuntimeGeometryCacheSceneProxy(UglTFRuntimeGeomCacheComponent* Component) : FGeometryCacheSceneProxy(Component, [this]() { return new FglTFRuntimeGeomCacheTrackProxy(GetScene().GetFeatureLevel()); })
	{
	}
};

FPrimitiveSceneProxy* UglTFRuntimeGeomCacheComponent::CreateSceneProxy()
{
	return new FglTFRuntimeGeometryCacheSceneProxy(this);
}

