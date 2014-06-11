'use strict';

angular.module('09ScreeninvaderApp')
  .controller('PlayercontrollerCtrl', function ($scope,$rootScope) {

    //############## Default settings ##############
    $scope.playerSetting = true;

    $scope.toggleSetting = function() {
      $scope.playerSetting = !$scope.playerSetting
    };

    $rootScope.$watch('model',function() {
      $scope.model = $rootScope.model;
    });


});
