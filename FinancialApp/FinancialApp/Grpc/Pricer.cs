using System.Net.Http;
using System.Threading.Tasks;

using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;
using FinancialApp.Utils.Serialization;

namespace FinancialApp.Grpc
{
    public class Pricer : IPricer
    {
       
        public PricingOutput Price(DateTime current_date, List<DataFeed> dataFeeds, TestParameters testParameters)
        {
            PricingInputSerializer serializer = new PricingInputSerializer();

            // Sérialisation
            PricingInput input = serializer.Serialize(dataFeeds, true, current_date);
        }
    }
}
// Initialisation des dépendances
//var grpcClient = new GrpcPricingClient("http://localhost:50051");
//var marketDataDeserializer = new MarketDataDeserializer();
//var financialParamsDeserializer = new ParameterJsonDeserialiser();
//var pricingInputSerializer = new PricingInputSerializer();
//private readonly GrpcPricer.GrpcPricerClient _client;

//public GrpcPricingClient(string serverAddress)
//{
//    var httpHandler = new HttpClientHandler
//    {
//        ServerCertificateCustomValidationCallback =
//            HttpClientHandler.DangerousAcceptAnyServerCertificateValidator
//    };

//    var channel = GrpcChannel.ForAddress(serverAddress, new GrpcChannelOptions { HttpHandler = httpHandler });
//    _client = new GrpcPricer.GrpcPricerClient(channel);
//}

//public Task<ReqInfo> GetHeartbeatAsync()
//{
//    return _client.HeartbeatAsync(new Empty()).ResponseAsync;
//}

//public Task<PricingOutput> GetPriceAndDeltasAsync(PricingInput input)
//{
//    return _client.PriceAndDeltasAsync(input).ResponseAsync;
//}