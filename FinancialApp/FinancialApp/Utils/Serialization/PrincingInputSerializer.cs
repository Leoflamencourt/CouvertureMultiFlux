using System;
using System.Collections.Generic;
using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;
using TimeHandler;

namespace FinancialApp.Utils.Serialization
{
    public class PricingInputSerializer : ISerializer<List<DataFeed>, PricingInput>
    {
        public PricingInput Serialize(List<DataFeed> subdataFeeds,  TestParameters parameters)
        {
            DateTime initialDate=subdataFeeds[0].Date;
            DateTime currentDate=subdataFeeds[-1].Date;
            MathDateConverter converter = new MathDateConverter(parameters.NumberOfDaysInOneYear);
            var pricingInput = new PricingInput
            {
                MonitoringDateReached = parameters.PayoffDescription.PaymentDates.Contains(currentDate),
                Time = converter.ConvertToMathDistance(currentDate, initialDate)
            };

            foreach (DataFeed dataFeed in subdataFeeds)
            {
                if (parameters.PayoffDescription.PaymentDates.Contains(dataFeed.Date))
                {
                    var pastLine = new PastLines();
                    pastLine.Value.AddRange(dataFeed.SpotList.Values);
                    pricingInput.Past.Add(pastLine);
                }
                
            }

            return pricingInput;
        }
    }
}
