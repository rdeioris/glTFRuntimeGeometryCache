// Copyright 2025, Roberto De Ioris.


#include "glTFRuntimeGeomCacheFuncLibrary.h"

UGeometryCache* UglTFRuntimeGeomCacheFuncLibrary::LoadGeometryCacheFromRuntimeTracks(const TArray<UglTFRuntimeGeometryCacheTrack*>& Tracks, const int32 FPS)
{
	if (Tracks.Num() < 1)
	{
		return nullptr;
	}

	UGeometryCache* GeometryCache = NewObject<UGeometryCache>(GetTransientPackage(), NAME_None, RF_Public);

	TSet<UMaterialInterface*> GlobalMaterials;

	float MaxDuration = 0.0f;
	int32 FrameEnd = 0;
	for (UglTFRuntimeGeometryCacheTrack* Track : Tracks)
	{
		GeometryCache->Tracks.Add(Track);

		GlobalMaterials.Append(Track->GetUniqueMeshMaterials());

		MaxDuration = FMath::Max(MaxDuration, Track->GetDuration());

		FrameEnd = FMath::Max(FrameEnd, Track->GetNumSampleInfos() - 1);
	}

	GeometryCache->Materials = GlobalMaterials.Array();
	if (GeometryCache->Materials.Num() < 1)
	{
		GeometryCache->Materials.Add(UMaterial::GetDefaultMaterial(EMaterialDomain::MD_Surface));
	}

	for (UglTFRuntimeGeometryCacheTrack* Track : Tracks)
	{
		Track->UpdateMeshMaterialsIndices(GeometryCache->Materials);
		Track->SetDuration(MaxDuration);
	}

	GeometryCache->SetFrameStartEnd(0, MaxDuration * FPS);

	return GeometryCache;
}

UglTFRuntimeGeometryCacheTrack* UglTFRuntimeGeomCacheFuncLibrary::LoadRuntimeTrackFromGeometryCacheFrames(const TArray<FglTFRuntimeGeometryCacheFrame>& Frames)
{
	if (Frames.Num() < 1)
	{
		return nullptr;
	}

	UglTFRuntimeGeometryCacheTrack* Track = NewObject<UglTFRuntimeGeometryCacheTrack>(GetTransientPackage(), NAME_None, RF_Public);

	for (const FglTFRuntimeGeometryCacheFrame& Frame : Frames)
	{
		TArray<UMaterialInterface*> MeshMaterials;
		Track->AddMatrixSample(Frame.Transform.ToMatrixWithScale(), Frame.Time);

		FGeometryCacheMeshData MeshData;
		// let's initialize to true, and then eventually change on missing data
		MeshData.VertexInfo.bHasTangentZ = true;
		MeshData.VertexInfo.bHasTangentX = true;
		MeshData.VertexInfo.bHasUV0 = true;

		MeshData.BoundingBox.Init();

		for (int32 PrimitiveIndex = 0; PrimitiveIndex < Frame.Mesh.Primitives.Num(); PrimitiveIndex++)
		{
			const FglTFRuntimePrimitive& Primitive = Frame.Mesh.Primitives[PrimitiveIndex];
			FGeometryCacheMeshBatchInfo BatchInfo;
			BatchInfo.MaterialIndex = PrimitiveIndex;
			BatchInfo.NumTriangles = Primitive.Indices.Num() / 3;
			BatchInfo.StartIndex = MeshData.Indices.Num();
			MeshData.BatchesInfo.Add(BatchInfo);

			MeshData.Indices.Append(Primitive.Indices);
			MeshData.Positions.Append(Primitive.Positions);

			if (Primitive.Normals.Num() < 1)
			{
				MeshData.VertexInfo.bHasTangentZ = false;
			}
			else
			{
				MeshData.TangentsZ.Append(Primitive.Normals);
			}

			if (Primitive.Tangents.Num() < 1)
			{
				MeshData.VertexInfo.bHasTangentX = false;
			}
			else
			{
				MeshData.TangentsX.Append(Primitive.Tangents);
			}

			if (Primitive.UVs.Num() > 0 && Primitive.UVs[0].Num() > 0)
			{
				MeshData.TextureCoordinates.Append(Primitive.UVs[0]);
			}
			else
			{
				MeshData.VertexInfo.bHasUV0 = false;
			}

			for (const FVector& Position : Primitive.Positions)
			{
				MeshData.BoundingBox += FVector3f(Position);
			}

			MeshMaterials.Add(Primitive.Material);
		}

		Track->AddMeshSample(MeshData, MeshMaterials, Frame.Time);
	}

	return Track;
}