// Copyright 2025, Roberto De Ioris.

#include "glTFRuntimeGeometryCacheTrack.h"
#include "GeometryCacheHelpers.h"
#include "Algo/BinarySearch.h"

const bool UglTFRuntimeGeometryCacheTrack::UpdateBoundsData(const float Time, const bool bLooping, const bool bIsPlayingBackward, int32& InOutBoundsSampleIndex, FBox& OutBounds)
{
	const int32 SampleIndex = GetSampleInfoIndex(Time, bLooping);
	if (SampleIndex < 0)
	{
		return false;
	}

	if (InOutBoundsSampleIndex == -1 || SampleIndex != InOutBoundsSampleIndex)
	{
		OutBounds = SampleInfos[SampleIndex].BoundingBox;
		InOutBoundsSampleIndex = SampleIndex;
		return true;
	}

	return false;
}

int32 UglTFRuntimeGeometryCacheTrack::GetSampleInfoIndex(const float Time, const bool bLooping)
{
	if (MeshSampleTimes.Num() == 0)
	{
		return -1;
	}

	if (MeshSampleTimes.Num() == 1)
	{
		return 0;
	}

	float SampleTime = Time;
	if (bLooping)
	{
		SampleTime = GeometyCacheHelpers::WrapAnimationTime(Time, GetDuration());
	}

	const int32 LastSampleIndex = MeshSampleTimes.Num() - 1;

	if (SampleTime < MeshSampleTimes[0])
	{
		return 0;
	}
	else if (SampleTime > MeshSampleTimes[LastSampleIndex])
	{
		return LastSampleIndex;
	}

	int32 SampleIndex = Algo::UpperBound(MeshSampleTimes, SampleTime);
	return (SampleIndex > 0) ? (SampleIndex - 1) : 0;
}

const FGeometryCacheTrackSampleInfo& UglTFRuntimeGeometryCacheTrack::GetSampleInfo(float Time, const bool bLooping)
{
	const int32 SampleIndex = GetSampleInfoIndex(Time, bLooping);
	if (SampleInfos.IsValidIndex(SampleIndex))
	{
		return SampleInfos[SampleIndex];
	}

	return FGeometryCacheTrackSampleInfo::EmptySampleInfo;
}

const bool UglTFRuntimeGeometryCacheTrack::UpdateMeshData(const float Time, const bool bLooping, int32& InOutMeshSampleIndex, FGeometryCacheMeshData*& OutMeshData)
{
	const int32 SampleIndex = GetSampleInfoIndex(Time, bLooping);

	if (InOutMeshSampleIndex == -1 || SampleIndex != InOutMeshSampleIndex)
	{
		if (GetMeshDataAtSampleIndex(SampleIndex, MeshData))
		{
			OutMeshData = &MeshData;
			InOutMeshSampleIndex = SampleIndex;
			return true;
		}
	}
	return false;
}

bool UglTFRuntimeGeometryCacheTrack::GetMeshDataAtSampleIndex(int32 SampleIndex, FGeometryCacheMeshData& OutMeshData)
{
	if (MeshSamples.IsValidIndex(SampleIndex))
	{
		OutMeshData = MeshSamples[SampleIndex];
		return true;
	}

	return false;
}

bool UglTFRuntimeGeometryCacheTrack::GetMeshDataAtTime(float Time, FGeometryCacheMeshData& OutMeshData)
{
	int32 SampleIndex = GetSampleInfoIndex(Time, false);
	return GetMeshDataAtSampleIndex(SampleIndex, OutMeshData);
}

bool UglTFRuntimeGeometryCacheTrack::IsTopologyCompatible(const int32 SampleIndexA, const int32 SampleIndexB) const
{
	return SampleInfos.IsValidIndex(SampleIndexA) && SampleInfos.IsValidIndex(SampleIndexB) && SampleInfos[SampleIndexA].NumVertices == SampleInfos[SampleIndexB].NumVertices;
}

void UglTFRuntimeGeometryCacheTrack::AddMeshSample(const FGeometryCacheMeshData& InMeshData, const TArray<UMaterialInterface*>& InMeshMaterials, const float SampleTime)
{
	MeshSamples.Add(InMeshData);
	MeshSampleTimes.Add(SampleTime);

	MeshMaterialsSamples.Add(InMeshMaterials);

	FGeometryCacheTrackSampleInfo SampleInfo(SampleTime, FBox(InMeshData.BoundingBox), InMeshData.Positions.Num(), InMeshData.Indices.Num());
	SampleInfos.Add(SampleInfo);

	Duration = FMath::Max(Duration, SampleTime);
}

TSet<UMaterialInterface*> UglTFRuntimeGeometryCacheTrack::GetUniqueMeshMaterials() const
{
	TSet<UMaterialInterface*> UniqueMaterials;

	for (const TArray<UMaterialInterface*>& MeshMaterialSamples : MeshMaterialsSamples)
	{
		UniqueMaterials.Append(MeshMaterialSamples);
	}

	return UniqueMaterials;
}

void UglTFRuntimeGeometryCacheTrack::UpdateMeshMaterialsIndices(const TArray<UMaterialInterface*>& InGlobalMaterials)
{
	for (int32 MeshDataSampleIndex = 0; MeshDataSampleIndex < MeshSamples.Num(); MeshDataSampleIndex++)
	{
		FGeometryCacheMeshData& CurrentMeshData = MeshSamples[MeshDataSampleIndex];
		for (FGeometryCacheMeshBatchInfo& CurrentMeshBatchInfo : CurrentMeshData.BatchesInfo)
		{
			int32 NewMaterialIndex = 0;
			if (MeshMaterialsSamples.IsValidIndex(MeshDataSampleIndex) && MeshMaterialsSamples[MeshDataSampleIndex].IsValidIndex(CurrentMeshBatchInfo.MaterialIndex))
			{
				int32 GlobalMaterialIndex = -1;
				if (InGlobalMaterials.Find(MeshMaterialsSamples[MeshDataSampleIndex][CurrentMeshBatchInfo.MaterialIndex], GlobalMaterialIndex) && GlobalMaterialIndex >= 0)
				{
					NewMaterialIndex = GlobalMaterialIndex;
				}
			}
			CurrentMeshBatchInfo.MaterialIndex = NewMaterialIndex;
		}
	}
}

int32 UglTFRuntimeGeometryCacheTrack::GetNumSampleInfos() const
{
	return SampleInfos.Num();
}

float UglTFRuntimeGeometryCacheTrack::GetMeshSampleTime(const int32 SampleIndex) const
{
	if (SampleIndex < 0)
	{
		return 0;
	}
	else if (SampleIndex >= MeshSampleTimes.Num())
	{
		if (MeshSampleTimes.Num() > 0)
		{
			return MeshSampleTimes[MeshSampleTimes.Num() - 1];
		}
	}
	return MeshSampleTimes[SampleIndex];
}