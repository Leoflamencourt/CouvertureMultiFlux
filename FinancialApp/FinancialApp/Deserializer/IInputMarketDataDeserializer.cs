using MarketData;

namespace FinancialApp.Deserializer

{
    public interface IInputMarketDataDeserializer
    {
        public List<DataFeed> Deserialize(string filePath);
    }
}