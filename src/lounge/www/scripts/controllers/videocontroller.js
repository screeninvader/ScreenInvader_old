'use strict';

angular.module('09ScreeninvaderApp')
  .controller('VideocontrollerCtrl', function ($scope,JanoshDriver,$rootScope) {

    $rootScope.$watch('model',function() {
      $scope.model = $rootScope.model;
    });

  });
