using System;
using System.Collections.Generic;
using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;
using TimeHandler;
using FinancialApp.Utils;

namespace FinancialApp.Utils.Serialization
{
    public class PricingInputSerializer : ISerializer<List<DataFeed>, PricingInput>
    {
        public PricingInput Serialize(List<DataFeed> subdataFeeds, TestParameters parameters)
        {
            DateTime initialDate = subdataFeeds[0].Date;
            DateTime currentDate = subdataFeeds[^1].Date; // Corrected indexing
            MathDateConverter converter = new MathDateConverter(parameters.NumberOfDaysInOneYear);
            List<PastLines> past = new List<PastLines>();
            var pricingInput = new PricingInput
            {
                MonitoringDateReached = parameters.PayoffDescription.PaymentDates.Contains(currentDate),
                Time = converter.ConvertToMathDistance(currentDate, initialDate),
            };
            pricingInput.Past.AddRange(ConvertToPastLines(ComputePastData.ComputePastLines(subdataFeeds, parameters)));
   
            return pricingInput;
        }

        private List<PastLines> ConvertToPastLines(List<DataFeed> subdataFeeds)
        {
            var pastLines = new List<PastLines>();

            foreach (var dataFeed in subdataFeeds)
            {
                var pastLine = new PastLines();
                pastLine.Value.AddRange(dataFeed.SpotList.Values);
                pastLines.Add(pastLine);
            }

            return pastLines;
        }
    }
}