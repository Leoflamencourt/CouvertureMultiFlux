using System.Collections.Generic;
using GrpcPricing.Protos;
using MarketData;

namespace FinancialApp.Utils.Serialization
{
    public class PricingInputSerializer : ISerializer<List<DataFeed>, PricingInput>
    {
        public PricingInput Serialize(List<DataFeed> dataFeeds, bool monitoringDateReached, DateTime time)
        {
            var pricingInput = new PricingInput
            {
                MonitoringDateReached = monitoringDateReached,
                Time= time
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
