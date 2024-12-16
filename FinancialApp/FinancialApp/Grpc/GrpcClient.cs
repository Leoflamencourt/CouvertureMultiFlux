using System.Net.Http;
using System.Threading.Tasks;
using Grpc.Net.Client;
using GrpcPricing.Protos;

namespace FinancialApp.Grpc
{
    public class GrpcPricingClient : IGrpcPricingClient
    {
        private readonly GrpcPricer.GrpcPricerClient _client;

        public GrpcPricingClient(string serverAddress)
        {
            var httpHandler = new HttpClientHandler
            {
                ServerCertificateCustomValidationCallback =
                    HttpClientHandler.DangerousAcceptAnyServerCertificateValidator
            };

            var channel = GrpcChannel.ForAddress(serverAddress, new GrpcChannelOptions { HttpHandler = httpHandler });
            _client = new GrpcPricer.GrpcPricerClient(channel);
        }

        public Task<ReqInfo> GetHeartbeatAsync()
        {
            return _client.HeartbeatAsync(new Empty()).ResponseAsync;
        }

        public Task<PricingOutput> GetPriceAndDeltasAsync(PricingInput input)
        {
            return _client.PriceAndDeltasAsync(input).ResponseAsync;
        }
    }
}
