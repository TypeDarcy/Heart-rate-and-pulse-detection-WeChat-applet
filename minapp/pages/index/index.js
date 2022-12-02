// pages/mine/mine.js

var util = require('../../utils/util.js')
const app = getApp()

Page({

  /**a
   * 页面的初始数据
   */
  data: {
    area: '请求中', //城区
    city: '请求中', //城市
    humidity: '请求中', //空气湿度
    temperature: 0, //温度
    weather: '请求中', //天气
    weatherAdvice: '疫情防控，人人有责', //天气建议
    Temp: '~', //体温
    Pulse: '~', //心率 
    Sao2: '~', //血氧
    //tabbar
    listInfo: [{
      text: '首页',
      imgUrl: '/images/index.png',
      curUrl: '/images/index-active.png',
    },
    {
      text: '服务',
      imgUrl: '/images/service.png',
      curUrl: '/images/service-active.png',
    },
    {
      text: '我的',
      imgUrl: '/images/mine.png',
      curUrl: '/images/mine-active.png',
    }
    ],

  },
  goTo(e) {
    var url = e.currentTarget.dataset.id
    wx.navigateTo({
      url: url,
    })
  },
  chooseImg: function (e) {
    var that = this
    this.setData({
      curIdx: e.currentTarget.dataset.current
    })
    console.log("kk", e.currentTarget.dataset.current)
    if (e.currentTarget.dataset.current == 0) {
      wx.switchTab({
        url: '/pages/index/index',
      })
    } else if (e.currentTarget.dataset.current == 1) {
      wx.switchTab({
        url: '/pages/service/service',
      })
    } else if (e.currentTarget.dataset.current == 2) {
      wx.switchTab({
        url: '/pages/mine/mine',
      })
    } else if (e.currentTarget.dataset.current == 3) {
      wx.switchTab({
        url: '/pages/person/person',
      })
    }
    that.onShow();
  },


  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {
    var curIdx = 0
    this.setData({
      curIdx: curIdx,
    })


  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady() {
    var that = this
    wx.getLocation({ //调用getLocation API获取经纬度
      type: 'gcj02',
      success(res) {
        const latitude = res.latitude //维度
        const longitude = res.longitude //经度
        const key = 'yourkey' //你的高德地图key
        console.log(res)
        wx.request({
          url: `https://restapi.amap.com/v3/geocode/regeo?output=json&location=${longitude},${latitude}&key=${key}&extensions=all`,
          success(res) {
            const adcode = res.data.regeocode.addressComponent.adcode //设置adcode（区域编码）
            console.log(res.data)
            that.setData({
              city: res.data.regeocode.addressComponent.province,
              area: res.data.regeocode.addressComponent.district,
            })
            wx.request({
              url: `https://restapi.amap.com/v3/weather/weatherInfo?city=${adcode}&key=${key}`,
              success(res) {
                console.log(res.data)
                that.setData({
                  //airText:res.data.data.quality,
                  temperature: res.data.lives[0].temperature + '℃',
                  weather: res.data.lives[0].weather,
                  humidity: res.data.lives[0].humidity,
                })
              }
            })
          }
        })


      }
    })
  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow() {

    var that = this
    app.globalData.client.on('message', function (topic, message) {
      console.log(topic)
      console.log(message) //message是16进制的Buffer字节流
      var dataFromDev = JSON.parse(message)
      console.log(dataFromDev)
      app.globalData.timelist = app.globalData.timelist.concat(util.formatTime(new Date())),
        app.globalData.Templist = app.globalData.Templist.concat(dataFromDev.params.Temp)
        app.globalData.Sao2list = app.globalData.Sao2list.concat(dataFromDev.params.SPO2)
        app.globalData.Hrlist = app.globalData.Hrlist.concat(dataFromDev.params.Pulse)
      that.setData({
        Temp: dataFromDev.params.Temp,
        Pulse: dataFromDev.params.Pulse,
        Sao2: dataFromDev.params.SPO2
      })

    })
    this.onLoad();
  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide() {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload() {

  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh() {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom() {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage() {

  }
})
