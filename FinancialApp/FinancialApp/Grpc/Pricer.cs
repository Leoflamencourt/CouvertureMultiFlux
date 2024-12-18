using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using Grpc.Net.Client;
using GrpcPricing.Protos;
using MarketData;
using ParameterInfo;
using FinancialApp.Utils.Serialization;

namespace FinancialApp.Grpc
{
    public class Pricer : IPricer
    {
        private readonly GrpcPricer.GrpcPricerClient _client;

        /// <summary>
        /// Constructeur pour initialiser le client gRPC.
        /// </summary>
        /// <param name="serverAddress">L'adresse du serveur gRPC.</param>
        public Pricer(string serverAddress)
        {
            var httpHandler = new HttpClientHandler
            {
                ServerCertificateCustomValidationCallback =
                    HttpClientHandler.DangerousAcceptAnyServerCertificateValidator
            };

            var channel = GrpcChannel.ForAddress(serverAddress, new GrpcChannelOptions { HttpHandler = httpHandler });
            _client = new GrpcPricer.GrpcPricerClient(channel);
        }

        /// <summary>
        /// Sérialise les données et récupère les PricingOutput en appelant le serveur gRPC.
        /// </summary>
        /// <param name="dataFeeds">Les données de marché.</param>
        /// <param name="currentDate">La date actuelle.</param>
        /// <param name="testParameters">Les paramètres de test.</param>
        /// <returns>Un objet PricingOutput contenant les résultats.</returns>
        public async Task<PricingOutput> PriceandDeltaAsync(List<DataFeed> subDataFeeds, DateTime currentDate, TestParameters testParameters)
        {
            // Sérialisation de l'entrée
            PricingInputSerializer serializer = new PricingInputSerializer();
            PricingInput input = serializer.Serialize(subDataFeeds, testParameters);
        

            try
            {
                // Appel au serveur gRPC pour obtenir les PricingOutput
                PricingOutput response = await _client.PriceAndDeltasAsync(input);
                return response;
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
