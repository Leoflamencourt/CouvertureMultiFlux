using FinancialApp.Deserializer;
using MarketData;


namespace FinancialApp.Parser
{
    public class MarketDataDeserializer : IInputMarketDataDeserializer
    {
        public List<DataFeed> Deserialize(string filePath)
        {
            return MarketDataReader.ReadDataFeeds(filePath);
        }

    }
}
