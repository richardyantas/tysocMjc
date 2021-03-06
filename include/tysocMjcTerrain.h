
#pragma once

#include <tysocMjcCommon.h>
#include <tysocMjcUtils.h>

// Pool size for the number of mjc bodies to use
#define MJC_TERRAIN_POOL_SIZE PROCEDURAL_TERRAIN_POOL_SIZE - 1
// Default parameters for the connected-paths section
#define MJC_TERRAIN_PATH_DEFAULT_WIDTH 0.5f
#define MJC_TERRAIN_PATH_DEFAULT_DEPTH 2.0f
#define MJC_TERRAIN_PATH_DEFAULT_TICKNESS 0.025f

#include <terrain/terrain.h>

namespace tysoc {
namespace mujoco {

    /**
    * This is a wrapper on top of the primitives...
    * used by the terrain generators
    */
    struct TMjcTerrainPrimitive
    {
        int                         mjcBodyId;
        std::string                 mjcBodyName;
        std::string                 mjcGeomType;
        TSizef                      mjcGeomSize;
        bool                        isAvailable;
        tysoc::terrain::TTerrainPrimitive*   tysocPrimitiveObj;
    };


    class TMjcTerrainGenWrapper
    {
        private :

        // Main storage for the primitives
        std::vector< TMjcTerrainPrimitive* > m_mjcTerrainPrimitives;
        // working queues for the logic
        std::queue< TMjcTerrainPrimitive* > m_mjcAvailablePrimitives;
        std::queue< TMjcTerrainPrimitive* > m_mjcWorkingPrimitives;
        std::queue< TMjcTerrainPrimitive* > m_mjcFixedPrimitives;

        // terrain generator to wrap
        tysoc::terrain::TITerrainGenerator* m_terrainGenPtr;

        // mujoco resources to inject into workspace
        mjcf::GenericElement* m_modelElmPtr;

        // a reference to the mujoco model
        mjModel* m_mjcModelPtr;
        mjData* m_mjcDataPtr;
        mjvScene* m_mjcScenePtr;

        // name for this agentwrapper (and underlying agent as well)
        std::string m_name;

        void _collectFromGenerator();// collects primitives that can be reused and rewrapped in the lifetime of the generator
        void _collectFixedFromGenerator();// collects primitives that are single in the lifetime of the generator
        void _wrapNewPrimitive( tysoc::terrain::TTerrainPrimitive* primitivePtr, bool isReusable );
        void _updateProperties( TMjcTerrainPrimitive* mjcTerrainPritimivePtr );

        public :

        TMjcTerrainGenWrapper( const std::string& name,
                               tysoc::terrain::TITerrainGenerator* terrainGenPtr );
        ~TMjcTerrainGenWrapper();

        void injectMjcResources( mjcf::GenericElement* root );
        void setMjcModel( mjModel* mjcModelPtr );
        void setMjcData( mjData* mjcDataPtr );
        void setMjcScene( mjvScene* mjcScenePtr );
        void initialize();

        std::string name() { return m_name; }
        tysoc::terrain::TITerrainGenerator* terrainGenerator() { return m_terrainGenPtr; }


        // update the wrapper by collecting all ...
        // information needed for stages on top of ...
        // the wrapper, like its concrete TTysocMjcApi parent
        void preStep();
    };

}}