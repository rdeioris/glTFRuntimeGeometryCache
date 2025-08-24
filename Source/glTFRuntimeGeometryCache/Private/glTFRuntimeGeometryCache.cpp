// Copyright 2025, Roberto De Ioris.

#include "glTFRuntimeGeometryCache.h"

#define LOCTEXT_NAMESPACE "FglTFRuntimeGeometryCacheModule"

void FglTFRuntimeGeometryCacheModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FglTFRuntimeGeometryCacheModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FglTFRuntimeGeometryCacheModule, glTFRuntimeGeometryCache)