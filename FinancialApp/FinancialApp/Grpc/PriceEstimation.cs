using System;
using System.Collections.Generic;

namespace FinancialApp.Grpc
{
    public class PriceEstimation
    {
        public Dictionary<string, double> Deltas { get; set; }
        public Dictionary<string, double> DeltaStdDevs { get; set; }
        public double Price { get; set; }
        public double PriceStdDev { get; set; }

        public PriceEstimation()
        {
            Deltas = new Dictionary<string, double>();
            DeltaStdDevs = new Dictionary<string, double>();
        }
    }
}
