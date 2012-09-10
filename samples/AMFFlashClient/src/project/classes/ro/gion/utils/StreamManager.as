package ro.gion.utils
{
    import flash.net.NetStream;
    
    public class StreamManager
    {
        private static var _streams:Object = {};
        
        public function getStream(name:String):NetStream
        {
            if (_streams[name] == null) {
                var stream:NetStream = new NetStream();
                _streams[name] = stream;
            }
            return _streams[name];
        }
    }
}
