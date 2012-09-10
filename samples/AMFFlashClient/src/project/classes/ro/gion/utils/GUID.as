package ro.gion.utils
{
    public class GUID
    {
        public static var CHARS:Array = [
            '0','1','2','3','4','5','6','7','8','9',
            'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
            'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'
        ];
        
        public static function uuid(len:Number, radix:Number):String {
            var chars:Array = CHARS;
            var uuid:Array = [];
            var radix:Number = radix || chars.length;
            var i:Number = 0;
            if (len) {
              // Compact form
              for (i = 0; i < len; i++) {
                uuid[i] = chars[0 | Math.random()*radix];
              }
            } else {
              // rfc4122, version 4 form
              var r:*;
              // rfc4122 requires these characters
              uuid[8] = uuid[13] = uuid[18] = uuid[23] = '-';
              uuid[14] = '4';
              // Fill in random data.  At i==19 set the high bits of clock sequence as
              // per rfc4122, sec. 4.1.5
              for (i = 0; i < 36; i++) {
                if (!uuid[i]) {
                  r = 0 | Math.random()*16;
                  uuid[i] = chars[(i == 19) ? (r & 0x3) | 0x8 : r];
                }
              }
            }
            return uuid.join('');
        };

        // A more performant, but slightly bulkier, RFC4122v4 solution.  We boost performance
        // by minimizing calls to random()
        public static function uuidFast():String {
            var chars:Array = CHARS, uuid:Array = new Array(36), rnd:Number=0, r:Number;
            for (var i:Number = 0; i < 36; i++) {
                if (i==8 || i==13 ||  i==18 || i==23) {
                    uuid[i] = '-';
                } else if (i==14) {
                    uuid[i] = '4';
                } else {
                    if (rnd <= 0x02) rnd = 0x2000000 + (Math.random()*0x1000000)|0;
                    r = rnd & 0xf;
                    rnd = rnd >> 4;
                    uuid[i] = chars[(i == 19) ? (r & 0x3) | 0x8 : r];
                }
            }
            return uuid.join('');
        };
        
    }
}
