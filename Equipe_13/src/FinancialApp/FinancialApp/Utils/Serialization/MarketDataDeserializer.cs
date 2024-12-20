using MarketData;

namespace FinancialApp.Utils.Serialization
{
    public class MarketDataDeserializer : IDeserializer<List<DataFeed>>
    {
        public List<DataFeed> Deserialize(string filePath)
        {
            return MarketDataReader.ReadDataFeeds(filePath);
        }
    }
}
