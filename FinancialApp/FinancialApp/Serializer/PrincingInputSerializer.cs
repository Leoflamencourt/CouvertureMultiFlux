using System.Collections.Generic;
using FinancialApp.Serializer;
using GrpcPricing.Protos;
using MarketData;

namespace FinancialApp.Serializers
{
    public class PricingInputSerializer : IPricingInputSerializer
    {
        public PricingInput Serialize(List<DataFeed> dataFeeds, bool monitoringDateReached, double time)
        {
            var pricingInput = new PricingInput
            {
                MonitoringDateReached = monitoringDateReached,
                Time = time
            };

            foreach (DataFeed dataFeed in dataFeeds)
            {
                var pastLine = new PastLines();
                pastLine.Value.AddRange((IEnumerable<double>)dataFeed.SpotList);
                pricingInput.Past.Add(pastLine);
            }

            return pricingInput;
        }
    }
}
