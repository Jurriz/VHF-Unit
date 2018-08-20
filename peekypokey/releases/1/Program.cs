using PeekyPokey;

namespace Blinky
{
    class Program
    {
        static void Main(string[] args)
        {
            // create a digital output port
            var LED = new OutputPort(Device.Pin.Gpio0);

            // loop
            while(true)
            {
                // turn LED on and wait 200ms
                LED.Value = true;
                System.Threading.Thread.Sleep(200);

                // turn the LED off and wait 200ms
                LED.Value = false;
                System.Threading.Thread.Sleep(200);
            }
        }
    }
}
