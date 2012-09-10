package ro.gion.clients
{
    import flash.media.Video;
    
    import flash.display.LoaderInfo;
    import flash.display.MovieClip;
    import flash.display.StageAlign;
    import flash.display.StageScaleMode;
    
    import flash.net.NetConnection;
    import flash.net.NetStream;
    import flash.net.Responder;
    
    import flash.system.Capabilities;
    
    import flash.events.AsyncErrorEvent;
    import flash.events.NetStatusEvent;
    import flash.events.Event;
    import flash.events.MouseEvent;
    
    import com.flashdynamix.utils.*;
    
    import org.as3commons.logging.*;
    import com.bit101.components.*;
    
    import ro.gion.utils.ObjectUtil;
    import ro.gion.utils.Enumerator;
    
    import ro.gion.clients.AMFFlashClientAssets;
    
    public class AMFFlashClient extends MovieClip
    {
        private static const logger: ILogger = getLogger(AMFFlashClient);
        private var parameters:Object;
        
        private var indicatorLight:IndicatorLight;
        private var callMethodButton:PushButton;
        private var gatewayURL:InputText;
        private var resource:InputText;
        private var debugArea:TextArea;
        
        private var nc:NetConnection;
        
        public function AMFFlashClient()
        {
            addEventListener(Event.ADDED_TO_STAGE, addedToStage_Handler);
        }
        
        private function addedToStage_Handler(event:Event):void
        {
            removeEventListener(Event.ADDED_TO_STAGE, addedToStage_Handler);
            parameters = LoaderInfo(this.root.loaderInfo).parameters;
            logger.info("Params are {0}", ObjectUtil.toString(parameters));
            AMFFlashClientAssets.initAssets();
            onReady();
        }
        
        public function onReady():void
        {
            logger.info("Ready");
            
            gatewayURL = new InputText(this, 5, 10);
            resource = new InputText(this, 5, gatewayURL.y + gatewayURL.height + 10);
            callMethodButton = new PushButton(this, 5, resource.y + resource.height + 10, "Call method", callMethod_Handler);
            
            gatewayURL.text = parameters.gatewayUrl;
            resource.text = parameters.resource == null ? '' : parameters.resource;
            
            debugArea = new TextArea(this, 5, callMethodButton.y + callMethodButton.height + 10);
            debugArea.html = true;
            debugArea.autoHideScrollBar = true;
            
            indicatorLight = new IndicatorLight(this, 5, debugArea.y + debugArea.height + 30, 0x333333, 'Connection status');
            indicatorLight.isLit = true;
            
            stage.align = StageAlign.TOP_LEFT;
            stage.scaleMode = StageScaleMode.NO_SCALE;
            stage.addEventListener(Event.RESIZE, resize_Handler);
            
            resize_Handler(null);
            SWFProfiler.init(stage, this);
            
            // Connect to the AMF gateway
            logger.info("Connecting to {0}", parameters.gatewayUrl);
            nc = new NetConnection();
            nc.client = this;
            nc.addEventListener(NetStatusEvent.NET_STATUS, netStatus_Handler);
            nc.connect(parameters.gatewayUrl);
        }
        
        protected function resize_Handler(e:Event):void
        {
            gatewayURL.width = stage.stageWidth - 10;
            resource.width = stage.stageWidth - 10;
            debugArea.width = stage.stageWidth - 10;
            callMethodButton.width = stage.stageWidth - 10;
            debugArea.height = stage.stageHeight - callMethodButton.y - callMethodButton.height - 40;
        }
        
        private function callMethod_Handler(event:MouseEvent):void
        {
            callEchoService();
        }
        
        private function netStatus_Handler(event:NetStatusEvent):void
        {
            indicatorLight.color = 0xFF0000;
            logger.info("Connection problems {0}", ObjectUtil.toString(event.info));
            debugArea.text += ObjectUtil.toString(event.info) + "<br>";
        }
        
        public function callEchoService():void
        {
            var responder:Responder = new Responder(this.getResult, this.getError);
            var args:Object = {};
            args['string'] = "String";
            args['number'] = 1.23;
            args['date'] = new Date();
            args['bool'] = true;
            args['array'] = new Array(1,2,3,4);
            args['object'] = {key1:"value1", key2:"value2"};
            nc.call(resource.text, responder, args);
        }
        
        public function getResult(obj:*):void
        {
            indicatorLight.color = 0x00FF00;
            logger.info("Request result {0}", obj);
            debugArea.text += "Result<br>" + ObjectUtil.toString(obj) + "<br>";
        }
        
        public function getError(obj:*):void
        {
            indicatorLight.color = 0xFF0000;
            logger.info("Request error {0}", obj);
            debugArea.text += "Error<br>" + ObjectUtil.toString(obj) + "<br>";
        }
    }
}
