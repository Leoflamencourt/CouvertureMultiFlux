using ParameterInfo.RebalancingOracleDescriptions;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace FinancialApp.Rebalancing
{
    public class FixedRebalancingOracle : IRebalancingOracleBase
    {
        private readonly int _rebalancingPeriod; // Période en jours
        private DateTime _lastRebalanceDate;

        public FixedRebalancingOracle(int rebalancingPeriod, DateTime startDate)
        {
            OracleType = RebalancingOracleType.Fixed;
            _rebalancingPeriod = rebalancingPeriod;
            _lastRebalanceDate = startDate;
        }

        public override bool ShouldRebalance(DateTime currentDate)
        {
            // Vérifie si la période de rebalancement est écoulée
            if ((currentDate - _lastRebalanceDate).TotalDays >= _rebalancingPeriod)
            {
                _lastRebalanceDate = currentDate; // Met à jour la dernière date
                return true; // Rebalancement nécessaire
            }
            return false;
        }
    }
}
