// Copyright Penkov G.S.

#include "Modules/ModuleManager.h"

class FMapGenerator : public FDefaultModuleImpl
{
public:
    virtual void StartupModule() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MapGenerator: Module loaded!"));
    }

    virtual void ShutdownModule() override
    {
        UE_LOG(LogTemp, Warning, TEXT("MapGenerator: Module unloaded!"));
    }
};

IMPLEMENT_MODULE(FMapGenerator, MapGenerator)