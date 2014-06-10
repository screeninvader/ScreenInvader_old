'use strict';

angular
  .module('09ScreeninvaderApp', [
    'ngResource',
    'ngSanitize',
    'angular-md5',
    'ui.router'
  ])
  .config(function($stateProvider,$urlRouterProvider,$sceDelegateProvider) {

    $urlRouterProvider.otherwise('/');

    var youtubeSearchView = {
         url:'search',
         views:{
               'content@':{
                  controller:'YoutubesearchCtrl',
                  templateUrl:'views/youtubesearch.html'
               }
         }
    };

    var postUrlView = {
         url:'posturl',
         views:{
               'content@':{
                  controller:'YoutubesearchCtrl',
                  templateUrl:'views/posturl.html'
               }
         }
    };

    var viewList = {
         url:'list',
         views:{
               'content@':{
                  controller: 'MainCtrl',
                  templateUrl:'views/main.html'
               }
         }
    };

    var viewSmall = {
         url:'small',
         views:{
               'content@':{
                  controller: 'MainCtrl',
                  templateUrl:'views/viewsmall.html'
               }
         }
    };

    var viewBig = {
         url:'big',
         views:{
               'content@':{
                  controller: 'MainCtrl',
                  templateUrl:'views/viewbig.html'
               }
         }
    };

    var viewThump = {
         url:'thumb',
         views:{
               'content@':{
                  controller: 'MainCtrl',
                  templateUrl:'views/viewthump.html'
               }
         }
    };

    var basicLayout = {
          url: '/',
          views:{
             'topBar':{
                controller: 'MainCtrl',
                templateUrl:'views/topbar.html'
             },
             'playerController':{ // todo wronge nameing
                controller: 'PlayercontrollerCtrl',
                templateUrl:'views/playercontroller.html'
             },
             'viewController':{ // todo wronge nameing
                controller: 'VideocontrollerCtrl',
                templateUrl:'views/videocontroller.html'
             },
             'content':{
                controller: 'MainCtrl',
                templateUrl:'views/main.html'
             },
             'footer':{
                controller : 'FooterCtrl',
                templateUrl:'views/footer.html'
             }
          }
    };

    var itemActions = {
        url: "action/:type/:id",
        onEnter: function($stateParams, $state,$rootScope,JanoshDriver) {

          var showNotification = function(context) {

            $rootScope.notify = {'context' : context, 'timestamp':Date.now()};

            // back to last
            var oldURL = location.href.split('#/')[1];
            var oldState = function() {
              if (oldURL === '') {
                return 'app'
              } else {
                return 'app.' + oldURL
              }
            }
            return $state.go(oldState());
          }

          switch($stateParams.type) {
            case 'del':
              if ($stateParams.id) {JanoshDriver.deleatItem($stateParams.id) }
              showNotification('Item will be deleated');
              break;
            case 'add':
              //$stateParams.id is the youtube Source
              if ($stateParams.id) {JanoshDriver.addItem($stateParams.id) }
              showNotification('Item will be played');
              break;
            case 'play':
              if ($stateParams.id) {JanoshDriver.playItem($stateParams.id) }
              showNotification('Item will be played');
              break;
            case 'stop':
              JanoshDriver.stop()
              showNotification('stop');
              break;
            case 'pause':
              JanoshDriver.pause();
              showNotification('pause');
              break;
           }
        }
    };

    var playerControll = {
        url:'controll/:type',
        onEnter: function($stateParams, $state,$rootScope,JanoshDriver) {

          var showNotification = function(context) {
            $rootScope.notify = {'context' : context, 'timestamp':Date.now()};
            // back to last
            var oldURL = location.href.split('#/')[1];
            var oldState = function() {
              if (oldURL === '') {
                return 'app'
              } else {
                return 'app.' + oldURL
              }
            }
            return $state.go(oldState());
          }

          switch($stateParams.type) {
            case 'del':
              if ($stateParams.id) {JanoshDriver.deleatItem($stateParams.id) }
              showNotification('Item will be deleated');
              break;
            case 'play':
              if ($stateParams.id) {JanoshDriver.playItem($stateParams.id) }
              showNotification('Item will be played');
              break;
            case 'step-backward':
              JanoshDriver.stepBackward();
              showNotification('step-backward');
              break;
            case 'fast-backward':
              JanoshDriver.fastBackward();
              showNotification('fast-backward');
              break;
            case 'backward':
              JanoshDriver.backward();
              showNotification('backward');
              break;
            case 'stop':
              JanoshDriver.stop();
              showNotification('stop');
              break;
            case 'pause':
              JanoshDriver.pause();;
              showNotification('pause');
              break;
            case 'forward':
              JanoshDriver.forward();
              showNotification('forward');
              break;
            case 'fast-forward':
              JanoshDriver.fastForward();
              showNotification('fast-forward');
              break;
            case 'step-forward':
              JanoshDriver.stepForward();
              showNotification('step-forward');
              break;
            case 'queue':
              JanoshDriver.toggleQueue();
              showNotification('queue');
              break;
            case 'browserClose':
              JanoshDriver.browserClose();
              showNotification('browserClose');
              break;
            case 'pdfClose':
              JanoshDriver.pdfClose();
              showNotification('pdfClose');
              break;
            case 'playerClose':
              JanoshDriver.playerClose();
              showNotification('playerClose');
              break;
            case 'volumeup':
              JanoshDriver.setSoundPlus();
              showNotification('setSoundPlus');
              break;
            case 'volumedown':
              JanoshDriver.setSoundMinus();
              showNotification('setSoundMinus');
              break;
            case 'mute':
              JanoshDriver.soundMute();
              showNotification('soundMute');
              break;
          }
        }
    };

    $stateProvider
       .state('app',basicLayout)
       .state('app.search',youtubeSearchView)
       .state('app.posturl',postUrlView)
       .state('app.action',itemActions)
       .state('app.playerControll',playerControll)
       .state('app.list',viewList)
       .state('app.small',viewSmall)
       .state('app.big',viewBig)
       .state('app.thumb',viewThump)
  })
  .run(function(JanoshDriver,$timeout,$rootScope){

/*    $rootScope.previousState = {};
    $rootScope.$on('$stateChangeSuccess',  function(event, toState, toParams, fromState, fromParams) {
        // store previous state in $rootScope
        $rootScope.previousState.name = fromState.name;
        $rootScope.previousState.params = fromParams;
    });*/

    $timeout(function() {
      JanoshDriver.getJanoshData();
      console.log('run');
    },0);

  });
