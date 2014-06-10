'use strict';

angular.module('09ScreeninvaderApp')
  .controller('NotifyCtrl', function ($scope,$timeout,$state,$stateParams,$rootScope,JanoshDriver) {
  $scope.isHidden = true;

  $scope.showNotification = function(context) {
    $scope.isHidden = false;
    $scope.contex = context;

    $timeout(function() {
      $scope.isHidden = true;
      if ($rootScope.previousState.name == '') {
        $state.go('app');
      } else {
        $state.go($rootScope.previousState.name);
      }
    },1500);
  }

  switch($stateParams.type) {
    case 'del':
      if ($stateParams.id) {JanoshDriver.deleatItem($stateParams.id) }
      $scope.showNotification('Item will be deleated');
      console.log($rootScope.previousState);
      break;
    case 'play':
      if ($stateParams.id) {JanoshDriver.playItem($stateParams.id) }
      $scope.showNotification('Item will be played');
      console.log($rootScope.previousState);
      break;
    case 'step-backward':
      JanoshDriver.stepBackward()
      $scope.showNotification('step-backward');
      console.log($rootScope.previousState);
      break;
    case 'fast-backward':
      JanoshDriver.fastBackward()
      $scope.showNotification('fast-backward');
      console.log($rootScope.previousState);
      break;
    case 'backward':
      JanoshDriver.backward()
      $scope.showNotification('backward');
      console.log($rootScope.previousState);
      break;
    case 'stop':
      JanoshDriver.stop()
      $scope.showNotification('stop');
      console.log($rootScope.previousState);
      break;
    case 'pause':
      JanoshDriver.pause();
      $scope.showNotification('pause');
      console.log($rootScope.previousState);
      break;
    case 'forward':
      JanoshDriver.forward()
      $scope.showNotification('forward');
      console.log($rootScope.previousState);
      break;
    case 'fast-forward':
      JanoshDriver.fastForward()
      $scope.showNotification('fast-forward');
      console.log($rootScope.previousState);
      break;
    case 'step-forward':
      JanoshDriver.stepForward()
      $scope.showNotification('step-forward');
      console.log($rootScope.previousState);
      break;
  }

});
