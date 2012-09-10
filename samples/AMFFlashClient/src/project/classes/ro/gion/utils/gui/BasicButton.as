package ro.gion.utils.gui 
{
    import flash.display.MovieClip;
    import flash.events.Event;
    import flash.events.MouseEvent;
    
    public class BasicButton extends MovieClip
    {
        protected var _isOver:Boolean = false;
        protected var _isDown:Boolean = false;
        protected var _isToggled:Boolean = false;
        protected var _isSelected:Boolean = false;
        
        public function BasicButton() 
        {
            addEventListener(Event.ADDED_TO_STAGE, addedToStage_Handler);
        }
        
        protected function addedToStage_Handler(e:Event):void
        {
            addEventListener(MouseEvent.MOUSE_DOWN, mouseDown_Handler);
            addEventListener(MouseEvent.MOUSE_OVER, mouseOver_Handler);
            addEventListener(MouseEvent.MOUSE_OUT, mouseOut_Handler);
            
            removeEventListener(Event.ADDED_TO_STAGE, addedToStage_Handler);
            addEventListener(Event.REMOVED_FROM_STAGE, removedFromStage_Handler);
        }
        
        protected function removedFromStage_Handler(e:Event):void
        {
            removeEventListener(MouseEvent.MOUSE_DOWN, mouseDown_Handler);
            removeEventListener(MouseEvent.MOUSE_UP, mouseUp_Handler);
            removeEventListener(MouseEvent.MOUSE_OVER, mouseOver_Handler);
            removeEventListener(MouseEvent.MOUSE_OUT, mouseOut_Handler);
            
            removeEventListener(Event.REMOVED_FROM_STAGE, removedFromStage_Handler);
        }
        
        public function mouseDown_Handler(e:MouseEvent):void
        {
            addEventListener(MouseEvent.MOUSE_UP, mouseUp_Handler);
            stage.addEventListener(MouseEvent.MOUSE_UP, mouseUp_Handler);
            
            _isDown = true;
            gotoAndStop("down");
        }
        
        public function mouseUp_Handler(e:MouseEvent):void
        {
            removeEventListener(MouseEvent.MOUSE_UP, mouseUp_Handler);
            stage.removeEventListener(MouseEvent.MOUSE_UP, mouseUp_Handler);
            
            _isDown = false;
            if (_isOver) {
                gotoAndStop("over");
            } else {
                if (_isToggled) {
                    gotoAndStop("toggled");
                } else {
                    gotoAndStop("up");
                }
            }
        }
        
        public function mouseOver_Handler(e:MouseEvent):void
        {
            _isOver = true;
            gotoAndStop("over");
        }
        
        public function mouseOut_Handler(e:MouseEvent):void
        {
            _isOver = false;
            if (_isDown) {
            } else {
                if (_isToggled) {
                    gotoAndStop("toggled");
                } else {
                    gotoAndStop("up");
                }
            }
        }
        
        public function set toggled(val:Boolean):void
        {
            this._isToggled = val;
            if (val) {
                gotoAndStop("toggled");
            } else {
                if (_isToggled) {
                    gotoAndStop("toggled");
                } else {
                    gotoAndStop("up");
                }
            }
        }
        
        public function get toggled():Boolean
        {
            return this._isToggled;
        }
    }

}