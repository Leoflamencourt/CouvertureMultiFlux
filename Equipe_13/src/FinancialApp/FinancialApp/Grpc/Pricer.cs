using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Grpc.Net.Client;
using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;
using FinancialApp.Utils.Serialization;
using FinancialApp.Utils;
using Utils;

namespace FinancialApp.Grpc
{
    public class Pricer : IPricer
    {
        private readonly GrpcPricer.GrpcPricerClient _client;
        private readonly GrpcChannel _channel;

        /// <summary>
        /// Constructeur pour initialiser le client gRPC.
        /// </summary>
        /// <param name="serverAddress">L'adresse du serveur gRPC.</param>
        public Pricer()
        {
            var httpHandler = new HttpClientHandler
            {
                ServerCertificateCustomValidationCallback =
                    HttpClientHandler.DangerousAcceptAnyServerCertificateValidator
            };

            _channel = GrpcChannel.ForAddress("http://localhost:50051", new GrpcChannelOptions { HttpHandler = httpHandler });
           
            _client = new GrpcPricer.GrpcPricerClient(_channel);
        }

        /// <summary>
        /// Sérialise les données et récupère les PricingOutput en appelant le serveur gRPC.
        /// </summary>
        /// <param name="dataFeeds">Les données de marché.</param>
        /// <param name="currentDate">La date actuelle.</param>
        /// <param name="testParameters">Les paramètres de test.</param>
        /// <returns>Un objet PricingOutput contenant les résultats.</returns>
        public async Task<PriceEstimation> PriceandDeltaAsync(List<DataFeed> subDataFeeds, DateTime currentDate, TestParameters testParameters)
        {
            // Sérialisation de l'entrée


            PricingInputSerializer serializer = new PricingInputSerializer();
            PricingInput input = serializer.Serialize(subDataFeeds, testParameters);
         
            
            
            PricingOutput response = await _client.PriceAndDeltasAsync(input);
            


            try
            {
               
                PriceEstimation result = new PriceEstimation
                {
                    Price = response.Price,
                    PriceStdDev = response.PriceStdDev,
                    Deltas = Util.TransformDelta(testParameters.PricingParams.UnderlyingPositions, response),
                    DeltaStdDevs = Util.TransformDeltaStdDev(testParameters.PricingParams.UnderlyingPositions, response)
                };
                
                return result;
            }
            catch (Exception ex)
            {
                // Gestion des erreurs (par exemple, journalisation)
                Console.WriteLine($"Erreur lors de l'appel gRPC : {ex.Message}");
                throw;
            }
        }
    }
}
