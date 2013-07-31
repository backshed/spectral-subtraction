using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;



namespace ApplyNoiseToAudio
{
    class AudioManager
    {

        double[] timit_raw_L = null;
        double[] timit_raw_R = null;


        public AudioManager()
        {
            FileStream ftimit = File.OpenRead(@"D:\TIMIT_norm.wav");
            WaveReader wavetimit = new WaveReader(ftimit);
            timit_raw_L = IntToDouble(wavetimit.Data[0]);


            TraverseTree(@"D:\noise\");
        }

        public void computeFile(String path)
        {
            // Read a sample noise file
            byte[] someBytes = File.ReadAllBytes(path);
            short[] someShorts = new short[someBytes.Length / 2];
            Buffer.BlockCopy(someBytes, 0, someShorts, 0, someBytes.Length);
            double[] noise = IntToDouble(someShorts);

            int[] reductionTab = new int[] { 0, 10, 20, 30, 50};
            foreach(int i in reductionTab)
            {
                String tmppath = path;
                // Execute;
                double[] output = applyNoiseToVoice(timit_raw_L, noise, i);

                // Output to file
                short[] outShorts = DoubleToShort(output);
                byte[] outBytes = new byte[2 * outShorts.Length];
                Buffer.BlockCopy(outShorts, 0, outBytes, 0, 2 * outShorts.Length);

                tmppath = tmppath.Remove(tmppath.Length - 4, 4);
                tmppath = tmppath.Remove(0, 3);
                tmppath = String.Concat(@"D:\ProcessedAudio\", tmppath);
                tmppath = String.Concat(tmppath, String.Concat("_", i.ToString(), "db.raw"));

                File.WriteAllBytes(tmppath, outBytes);
            }

        }

        public double[] IntToDouble(Int32[] array)
        {
            double[] output = new double[array.Length];

            for (int i = 0; i < array.Length; ++i)
            {
                output[i] = Convert.ToDouble(array[i]) / 32768.0;
            }

            return output;
        }

        public double[] IntToDouble(Int16[] array)
        {
            double[] output = new double[array.Length];

            for (int i = 0; i < array.Length; ++i)
            {
                output[i] = Convert.ToDouble(array[i]) / 32768.0;
            }

            return output;
        }

        public Int16[] DoubleToShort(double[] array)
        {
            Int16[] output = new short[array.Length];

            for (int i = 0; i < array.Length; ++i)
            {
                output[i] = Convert.ToInt16(array[i] * 32767); //Int16 goes from -2^(N-1) to 2^(N-1)-1
            }

            return output;
        }
        public double[] normalize(double[] array)
        {
            // 1. Search max.
            double max = 0;

            for (int i = 0; i < array.Length; ++i)
            {
                if (Math.Abs(array[i]) > max)
                    max = Math.Abs(array[i]);
            }

            // 2. Multiply by 1 / max
            for (int i = 0; i < array.Length; ++i)
            {
                array[i] *= 1 / max;
            }

            return array;
        }

        public double[] applyNoiseToVoice(double[] voice, double[] noise, int db)
        {
            double[] tab = new double[voice.Length];
            normalize(noise);
            double reductionFactor = Math.Pow(10.0, -db / 20.0);
            int index_in_noise = 0;
            for (int i = 0; i < voice.Length; ++i)
            {
                tab[i] = (voice[i] + (noise[index_in_noise] * reductionFactor)) / 2.0;
                index_in_noise++;
                if (index_in_noise > noise.Length - 1)
                    index_in_noise = 0;
            }
            return normalize(tab);
        }

        public void TraverseTree(string root)
        {
            Stack<string> dirs = new Stack<string>(20);

            if (!System.IO.Directory.Exists(root))
            {
                throw new ArgumentException();
            }
            dirs.Push(root);

            while (dirs.Count > 0)
            {
                string currentDir = dirs.Pop();
                string[] subDirs;
                try
                {
                    subDirs = System.IO.Directory.GetDirectories(currentDir);
                }
                catch (UnauthorizedAccessException e)
                {
                    Console.WriteLine(e.Message);
                    continue;
                }
                catch (System.IO.DirectoryNotFoundException e)
                {
                    Console.WriteLine(e.Message);
                    continue;
                }

                string[] files = null;
                try
                {
                    files = System.IO.Directory.GetFiles(currentDir);
                }

                catch (UnauthorizedAccessException e)
                {

                    Console.WriteLine(e.Message);
                    continue;
                }

                catch (System.IO.DirectoryNotFoundException e)
                {
                    Console.WriteLine(e.Message);
                    continue;
                }


                foreach (string file in files)
                {
                    try
                    {
                        System.IO.FileInfo fi = new System.IO.FileInfo(file);
                        if (fi.Extension == ".raw")
                        {
                            Console.WriteLine("Computing: {0}", file);
                            computeFile(fi.FullName);
                        }
                    }
                    catch (System.IO.FileNotFoundException e)
                    {
                        Console.WriteLine(e.Message);
                        continue;
                    }
                }

                foreach (string str in subDirs)
                    dirs.Push(str);
            }
        }
    }
}
