'use strict';

angular.module('09ScreeninvaderApp')
  .controller('MainCtrl', function ($scope,JanoshDriver,$rootScope,$timeout) {

    $rootScope.$watch('model',function(data) {
      $scope.model = $rootScope.model;
    });

    $rootScope.$watch('notify',function(data) {
      if (typeof $scope.notify !== 'undefined') {
        if ($scope.notify.timestamp !== $rootScope.notify.timestamp) {
        console.log($scope.notify);
          //new notify message is send
          $scope.notify = $rootScope.notify;
          // after 3 sec the context message is reset
          console.log('test');

        }
      }
    });

    $scope.getUrl = function(key) {
      $('#'+key).toggleClass('hide').focus();
    };

});
