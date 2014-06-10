'use strict';

angular.module('09ScreeninvaderApp')
  .controller('YoutubesearchCtrl', function ($scope,$http,$timeout,JanoshDriver) {

  $scope.getUrl = function(key) {
    $('#'+key).toggleClass('hide').focus();
  };

  $scope.youtubeAPI = function() {
    var max_videos = 6;
    var parameters = {
      params: {
          type: 'video',
          'max-results':max_videos,
          alt:'json',
          q: $scope.query
        }
      };
    var youtubeApiV3Url = 'https://www.googleapis.com/youtube/v3/search'; // require key parameter
    var youtubeApiV1Url = "http://gdata.youtube.com/feeds/api/videos";

    $http.get(youtubeApiV1Url,parameters)
      .success( function (result) {
        var data = _.map(result.feed.entry, function (num,key){
            var a = num.id.$t.split("/"),
                id = a[6],
                title = num.title.$t,
                thumbnail = num.media$group.media$thumbnail[1].url,
                description = num.media$group.media$description.$t;
            return {
              source:"http://www.youtube.com/watch?v="+id,
              youtubeId:id,
              title:title,
              thumbnail:thumbnail,
              description:description
            };
        });
        $scope.YoutubeResults = data;
        //console.log($scope.YoutubeResults);
      })
      .error(function(result) {
        console.log(result);
      });
  };

  $scope.postUrl = function() {
    JanoshDriver.addItem($scope.postUrlQuery)
  }

  $scope.acceptTypes = 'video/*,audio/*,image/*';

  $scope.onUpload = function(file) {
    console.log('upload',file);
    $scope.message = file[0].name + ' will be uploaded';
  }

  $scope.onError = function(res) {
    console.log('error',res);
  }

  $scope.onSuccess = function(res) {
    console.log('success',res);
  }

  $scope.onComplete = function(res) {
    console.log('Complete',res);
    $scope.message = 'Upload success';
  }


});
