// pages/mine/mine.js

Page({
  
  /**a
   * 页面的初始数据
   */
  data: {

    
    //用户信息
    userName : '游客',
    userImg : '/images/userImg.png',
    btn : '登录',

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
    }],
  },
  getInfo:function(){
    wx.getUserInfo({
      success:(res)=>{
        this.setData({
          userImg : res.userInfo.avatarUrl,
          userName : res.userInfo.nickName,
          btn : '退出'
          
        })
      }
    })
  },
    chooseImg: function(e) {
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
  
    goTo(e) {
      var url = e.currentTarget.dataset.id
      wx.navigateTo({
        url: url,
      })
    },
  /**
   * 生命周期函数--监听页面加载
   */
  onLoad(options) {
    var curIdx = 2
    this.setData({
      curIdx: curIdx,
    })


  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady() {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow() {
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