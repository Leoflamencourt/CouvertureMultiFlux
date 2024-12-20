using ParameterInfo.RebalancingOracleDescriptions;
using System;

namespace FinancialApp.Rebalancing
{
    public class FixedRebalancingOracle : IRebalancingOracleBase
    {
        private readonly int _rebalancingPeriod; 
        private int _callCounter;

        public FixedRebalancingOracle(int rebalancingPeriod)
        {
            OracleType = RebalancingOracleType.Fixed;
            _rebalancingPeriod = rebalancingPeriod;
            _callCounter = 0;
        }

        public override bool ShouldRebalance(DateTime currentDate)
        {
            _callCounter++;

          
            if (_callCounter >= _rebalancingPeriod)
            {
                _callCounter = 0; 
                return true; 
            }
            return false; 
        }
    }
}
