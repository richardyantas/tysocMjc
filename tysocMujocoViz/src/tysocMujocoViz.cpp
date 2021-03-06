
#include <tysocMujocoViz.h>



namespace tysoc {
namespace viz {

    TMujocoVisualizer* TMujocoVisualizer::_INSTANCE = NULL;

    TMujocoVisualizer::TMujocoVisualizer( TTysocCommonApi* tysocApiPtr )
        : TIVisualizer( tysocApiPtr )
    {
        if ( _INSTANCE )
        {
            std::cout << "WARNING> already created an instance of mjc visualizer" << std::endl;
            return;
        }

        _INSTANCE = this;

        m_mjcModelPtr       = NULL;
        m_mjcDataPtr        = NULL;
        m_mjcScenePtr       = NULL;
        m_mjcCameraPtr      = NULL;
        m_mjcOptionPtr      = NULL;
        m_mjcGpuContextPtr  = NULL;

        m_glfwWindowPtr     = NULL;

        m_demoInfo.buttonLeftPressed    = false;
        m_demoInfo.buttonMiddlePressed  = false;
        m_demoInfo.buttonRightPressed   = false;
        m_demoInfo.lastX = false;
        m_demoInfo.lastY = false;

        // This visualizer should only work with the tysoc mujoco API type
        if ( m_tysocApiPtr->getApiType() != API_TYPE_MUJOCO )
        {
            std::cout << "ERROR> Trying to use mujoco visualizer with a different physics backend" << std::endl;
            std::cout << "ERROR> mujoco API defines the visualizer functionality as well, and it's coupled with the whole library" << std::endl;
        }
    }

    TMujocoVisualizer::~TMujocoVisualizer()
    {
        // @TODO: implement the release functionality
        if ( m_mjcGpuContextPtr )
        {
            mjr_freeContext( m_mjcGpuContextPtr );
            m_mjcGpuContextPtr = NULL;
        }
    }

    void TMujocoVisualizer::setMjcModel( mjModel* mjcModelPtr )
    {
        m_mjcModelPtr = mjcModelPtr;
    }

    void TMujocoVisualizer::setMjcData( mjData* mjcDataPtr )
    {
        m_mjcDataPtr = mjcDataPtr;
    }

    void TMujocoVisualizer::setMjcScene( mjvScene* mjcScenePtr )
    {
        m_mjcScenePtr = mjcScenePtr;
    }

    void TMujocoVisualizer::setMjcCamera( mjvCamera* mjcCameraPtr )
    {
        m_mjcCameraPtr = mjcCameraPtr;
    }    

    void TMujocoVisualizer::setMjcOption( mjvOption* mjcOptionPtr )
    {
        m_mjcOptionPtr = mjcOptionPtr;
    }

    void TMujocoVisualizer::_initializeInternal()
    {
        // Initialize glfw
        if ( !glfwInit() )
        {
            std::cout << "ERROR> Could not initialize GLFW" << std::endl;
            return;
        }

        m_glfwWindowPtr = glfwCreateWindow( 1024, 768, "Mujoco Visualizer", NULL, NULL );
        if ( !m_glfwWindowPtr )
        {
            glfwTerminate();
            std::cout << "ERROR> Could not create glfw window" << std::endl;
            return;
        }

        glfwMakeContextCurrent( m_glfwWindowPtr );
        glfwSwapInterval( 1 );

        // Initialize mujoco rendering context
        m_mjcGpuContextPtr  = new mjrContext();
        mjr_makeContext( m_mjcModelPtr, m_mjcGpuContextPtr, mjFONTSCALE_150 );
        // Initialize mouse and scroll callbacks
        glfwSetKeyCallback( m_glfwWindowPtr, TMujocoVisualizer::KeybackCallback );
        glfwSetMouseButtonCallback( m_glfwWindowPtr, TMujocoVisualizer::MouseButtonCallback );
        glfwSetCursorPosCallback( m_glfwWindowPtr, TMujocoVisualizer::MouseMoveCallback );
        glfwSetScrollCallback( m_glfwWindowPtr, TMujocoVisualizer::ScrollCallback );

        //// and finally create the UI
        // first the ui context
        m_uiContextPtr = new TMujocoContextUI();
        m_uiContextPtr->isUiActive          = false;
        m_uiContextPtr->isBasicUiActive     = true;
        m_uiContextPtr->glfwWindowPtr       = m_glfwWindowPtr;
        // and then the UI
        m_uiPtr = new TMujocoUI( m_tysocApiPtr,
                                 m_uiContextPtr );
        m_uiPtr->initUI();
    }

    void TMujocoVisualizer::_updateInternal()
    {
        if ( !m_glfwWindowPtr )
        {
            return;
        }

        // get framebuffer viewport
        mjrRect _viewport = { 0, 0, 0, 0 };
        glfwGetFramebufferSize( m_glfwWindowPtr, &_viewport.width, &_viewport.height );

        // render
        mjr_render( _viewport, m_mjcScenePtr, m_mjcGpuContextPtr );

        // render the UI
        _renderUIInternal();

        glfwSwapBuffers( m_glfwWindowPtr );
        glfwPollEvents();
    }

    void TMujocoVisualizer::_renderUIInternal()
    {
        m_uiPtr->renderUI();
    }

    bool TMujocoVisualizer::_isActiveInternal()
    {
        return !glfwWindowShouldClose( m_glfwWindowPtr );
    }

    TIVizCamera* TMujocoVisualizer::_createCameraInternal( const std::string& name,
                                                           const std::string& type,
                                                           const TVec3& pos,
                                                           const TMat3& rot )
    {
        // @TODO|@WIP
        return NULL;
    }

    void TMujocoVisualizer::_changeToCameraInternal( TIVizCamera* cameraPtr )
    {
        // @TODO|@WIP
    }

    void TMujocoVisualizer::_grabCameraFrameInternal( TIVizCamera* cameraPtr,
                                                      TIVizTexture& rgbTexture,
                                                      TIVizTexture& depthTexture )
    {
        // @TODO|@WIP
    }

    TIVizLight* TMujocoVisualizer::_createLightInternal( const std::string& name,
                                                         const std::string& type,
                                                         const TVec3& pos )
    {
        // @TODO|@WIP
        return NULL;
    }

    // GLFW registered callbacks *************************************************************

    void TMujocoVisualizer::KeybackCallback( GLFWwindow* windowPtr, 
                                             int keyCode,
                                             int scanCode,
                                             int action,
                                             int modifiers )
    {
        if ( TMujocoVisualizer::_INSTANCE )
        {
            TMujocoVisualizer::_INSTANCE->_onKeyCallback( keyCode, action, modifiers );
        }
    }

    void TMujocoVisualizer::MouseButtonCallback( GLFWwindow* windowPtr,
                                                 int button,
                                                 int action,
                                                 int modifiers )
    {
        if ( TMujocoVisualizer::_INSTANCE )
        {
            TMujocoVisualizer::_INSTANCE->_onMouseButtonCallback( button, action, modifiers );
        }
    }

    void TMujocoVisualizer::MouseMoveCallback( GLFWwindow* windowPtr,
                                               double x,
                                               double y )
    {
        if ( TMujocoVisualizer::_INSTANCE )
        {
            TMujocoVisualizer::_INSTANCE->_onMouseMoveCallback( x, y );
        }
    }

    void TMujocoVisualizer::ScrollCallback( GLFWwindow* windowPtr,
                                            double xOffset,
                                            double yOffset )
    {
        if ( TMujocoVisualizer::_INSTANCE )
        {
            TMujocoVisualizer::_INSTANCE->_onScrollCallback( xOffset, yOffset );
        }
    }

    // Event callbacks ***********************************************************************

    void TMujocoVisualizer::_onKeyCallback( int keyCode, int action, int modifiers )
    {
        if ( keyCode == GLFW_KEY_SPACE )
        {
            // @DIRTY: enable UI
            m_uiContextPtr->isUiActive = true;
        }
        else if ( keyCode == GLFW_KEY_ENTER )
        {
            // @DIRTY: disable UI
            m_uiContextPtr->isUiActive = false;
        }
    }

    void TMujocoVisualizer::_onMouseButtonCallback( int button, int action, int modifiers )
    {
        if ( m_uiContextPtr->isUiActive )
        {
            return;
        }

        // update button state
        m_demoInfo.buttonLeftPressed    = ( glfwGetMouseButton( m_glfwWindowPtr, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS );
        m_demoInfo.buttonMiddlePressed  = ( glfwGetMouseButton( m_glfwWindowPtr, GLFW_MOUSE_BUTTON_MIDDLE ) == GLFW_PRESS );
        m_demoInfo.buttonRightPressed   = ( glfwGetMouseButton( m_glfwWindowPtr, GLFW_MOUSE_BUTTON_RIGHT ) == GLFW_PRESS );

        // update mouse position
        glfwGetCursorPos( m_glfwWindowPtr, &m_demoInfo.lastX, &m_demoInfo.lastY );
    }

    void TMujocoVisualizer::_onMouseMoveCallback( double x, double y )
    {
        if ( m_uiContextPtr->isUiActive )
        {
            return;
        }

        // no buttons down: nothing to do
        if( !m_demoInfo.buttonLeftPressed && 
            !m_demoInfo.buttonMiddlePressed && 
            !m_demoInfo.buttonRightPressed )
            return;

        // compute mouse displacement, save
        double _dx = x - m_demoInfo.lastX;
        double _dy = y - m_demoInfo.lastY;
        m_demoInfo.lastX = x;
        m_demoInfo.lastY = y;

        // get current window size
        int _width, _height;
        glfwGetWindowSize( m_glfwWindowPtr, &_width, &_height );

        // get shift key state
        bool _modShift = ( glfwGetKey( m_glfwWindowPtr, GLFW_KEY_LEFT_SHIFT ) == GLFW_PRESS ||
                           glfwGetKey( m_glfwWindowPtr, GLFW_KEY_RIGHT_SHIFT ) == GLFW_PRESS );

        // determine action based on mouse button
        mjtMouse _action;
        if( m_demoInfo.buttonRightPressed )
            _action = _modShift ? mjMOUSE_MOVE_H : mjMOUSE_MOVE_V;
        else if( m_demoInfo.buttonLeftPressed )
            _action = _modShift ? mjMOUSE_ROTATE_H : mjMOUSE_ROTATE_V;
        else
            _action = mjMOUSE_ZOOM;

        // move camera
        mjv_moveCamera( m_mjcModelPtr, 
                        _action, _dx / _height, _dy / _height, 
                        m_mjcScenePtr, 
                        m_mjcCameraPtr );
    }

    void TMujocoVisualizer::_onScrollCallback( double xOffset, double yOffset )
    {
        if ( m_uiContextPtr->isUiActive )
        {
            return;
        }

        // emulate vertical mouse motion = 5% of window height
        mjv_moveCamera( m_mjcModelPtr,
                        mjMOUSE_ZOOM, 0, -0.05 * yOffset, 
                        m_mjcScenePtr, 
                        m_mjcCameraPtr );
    }


}}